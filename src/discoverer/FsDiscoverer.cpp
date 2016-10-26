/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <algorithm>
#include <queue>

#include "MediaDevice.h"
#include "MediaFile.h"
#include "MediaFolder.h"
#include "FsDiscoverer.h"
#include "Media.h"
#include "MediaExplorer.h"
#include "filesystem/IDevice.h"
#include "filesystem/IDirectory.h"
#include "logging/Logger.h"

mxp::FsDiscoverer::FsDiscoverer(FileSystemPtr fsFactory, mxp::MediaExplorer* ml, mxp::IMediaExplorerCb* cb)
  : m_ml(ml)
  , m_fsFactory(fsFactory)
  , m_cb(cb) {}

bool mxp::FsDiscoverer::Discover(const std::string& entryPoint) {
  LOG_INFO("Adding to discovery list: ", entryPoint);
  // Assume :// denotes a scheme that isn't a file path, and refuse to discover it.
  if (entryPoint.find("://") != std::string::npos)
    return false;

  auto fsDir = m_fsFactory->CreateDirectoryFromPath(entryPoint);
  // Otherwise, create a directory and check it for modifications
  if (fsDir == nullptr) {
    LOG_ERROR("Failed to create an IDirectory for ", entryPoint);
    return false;
  }
  auto f = mxp::MediaFolder::FindByPath(m_ml, fsDir->path());
  // If the folder exists, we assume it will be handled by reload()
  if (f != nullptr)
    return true;
  if (hasDotNoMediaFile(*fsDir))
    return true;
  return addFolder(*fsDir, nullptr);
}

void mxp::FsDiscoverer::reload() {
  LOG_INFO("Reloading all folders");
  // Start by checking if previously known devices have been plugged/unplugged
  checkDevices();
  auto rootFolders = mxp::MediaFolder::FetchAll(m_ml, 0);
  
  for (const auto& f : rootFolders) {
    auto folder = m_fsFactory->CreateDirectoryFromPath(f->path());
    if (folder == nullptr) {
      m_ml->DeleteMediaFolder(*f);
      continue;
    }
    checkFolder(*folder, *f);
  }
}

void mxp::FsDiscoverer::reload(const std::string& entryPoint) {
  LOG_INFO("Reloading folder ", entryPoint);
  auto folder = mxp::MediaFolder::FindByPath(m_ml, entryPoint);
  if (folder == nullptr) {
    LOG_ERROR("Can't reload ", entryPoint, ": folder wasn't found in database");
    return;
  }
  auto folderFs = m_fsFactory->CreateDirectoryFromPath(folder->path());
  if (folderFs == nullptr) {
    LOG_ERROR(" Failed to create a fs::IDirectory representing ", folder->path());
  }
  checkFolder(*folderFs, *folder);
}

void mxp::FsDiscoverer::checkDevices() {
  m_fsFactory->Refresh();
  auto devices = MediaDevice::FetchAll(m_ml);
  for (auto& d : devices) {
    auto deviceFs = m_fsFactory->CreateDevice(d->uuid());
    auto fsDevicePresent = deviceFs != nullptr && deviceFs->isPresent();
    if (d->isPresent() != fsDevicePresent) {
      LOG_INFO("Device ", d->uuid(), " changed presence state: ", d->isPresent(), " -> ", fsDevicePresent);
      d->setPresent(fsDevicePresent);
    } else {
      LOG_INFO("Device ", d->uuid(), " unchanged");
    }
  }
}

void mxp::FsDiscoverer::checkFolder(mxp::fs::IDirectory& currentFolderFs, mxp::MediaFolder& currentFolder) const {
  // We already know of this folder, though it may now contain a .nomedia file.
  // In this case, simply delete the folder.
  if (hasDotNoMediaFile(currentFolderFs)) {
    LOG_INFO("Deleting folder ", currentFolderFs.path(), " due to a .nomedia file");
    m_ml->DeleteMediaFolder(currentFolder);
    return;
  }
  if (m_cb != nullptr)
    m_cb->onDiscoveryProgress(currentFolderFs.path());
  // Load the folders we already know of:
  LOG_INFO("Checking for modifications in ", currentFolderFs.path());
  auto subFoldersInDB = mxp::MediaFolder::FetchAll(m_ml, currentFolder.Id());
  for (const auto& subFolder : currentFolderFs.dirs()) {
    auto it = std::find_if(begin(subFoldersInDB), end(subFoldersInDB), [&subFolder](const std::shared_ptr<mxp::MediaFolder>& f) {
      return f->path() == subFolder->path();
    });
    // We don't know this folder, it's a new one
    if (it == end(subFoldersInDB)) {
      if (hasDotNoMediaFile(*subFolder)) {
        LOG_INFO("Ignoring folder with a .nomedia file");
        continue;
      }
      LOG_INFO("New folder detected: ", subFolder->path());
      try {
        addFolder(*subFolder, &currentFolder);
        continue;
      } catch (mxp::sqlite::errors::ConstraintViolation& ex) {
        // Best attempt to detect a foreign key violation, indicating the parent folders have been
        // deleted due to blacklisting
        if (strstr(ex.what(), "foreign key") != nullptr) {
          LOG_WARN("Creation of a folder failed because the parent is non existing: ", ex.what(),
            ". Assuming it was deleted due to blacklisting");
          return;
        }
        LOG_WARN("Creation of a duplicated folder failed: ", ex.what(), ". Assuming it was blacklisted");
        continue;
      }
    }
    auto folderInDb = *it;
    // In any case, check for modifications, as a change related to a mountpoint might
    // not update the folder modification date.
    // Also, relying on the modification date probably isn't portable
    checkFolder(*subFolder, *folderInDb);
    subFoldersInDB.erase(it);
  }
  // Now all folders we had in DB but haven't seen from the FS must have been deleted.
  for (auto f : subFoldersInDB) {
    LOG_INFO("Folder ", f->path(), " not found eviceFs->uuid()in FS, deleting it");
    m_ml->DeleteMediaFolder(*f);
  }
  checkFiles(currentFolderFs, currentFolder);
  LOG_INFO("Done checking subfolders in ", currentFolderFs.path());
}

void mxp::FsDiscoverer::checkFiles(mxp::fs::IDirectory& parentFolderFs, mxp::MediaFolder& parentFolder) const {
  LOG_INFO("Checking file in ", parentFolderFs.path());
  static const std::string req = "SELECT * FROM " + policy::MediaFileTable::Name
      + " WHERE folder_id = ?";
  auto files = mxp::MediaFile::FetchAll<mxp::MediaFile>(m_ml, req, parentFolder.Id());
  std::vector<std::shared_ptr<mxp::fs::IFile>> filesToAdd;
  std::vector<std::shared_ptr<mxp::MediaFile>> filesToRemove;
  for (const auto& fileFs: parentFolderFs.Files()) {
    auto it = std::find_if(begin(files), end(files), [fileFs](const std::shared_ptr<mxp::MediaFile>& f) {
      return f->mrl() == fileFs->GetFullPath();
    });
    if (it == end(files)) {
      filesToAdd.push_back(fileFs);
      continue;
    }
    if (fileFs->GetLastModificationDate() == (*it)->LastModificationDate()) {
      // Unchanged file
      files.erase(it);
      continue;
    }
    auto& file = (*it);
    LOG_INFO("Forcing file refresh ", fileFs->GetFullPath());
    // Pre-cache the file's media, since we need it to remove. However, better doing it
    // out of a write context, since that way, other threads can also read the database.
    file->media();
    filesToRemove.push_back(file);
    filesToAdd.push_back(fileFs);
    files.erase(it);
  }

  auto t = m_ml->GetConnection()->NewTransaction();
  for (auto file : files) {
    LOG_INFO("MediaFile ", file->mrl(), " not found on filesystem, deleting it");
    file->media()->RemoveFile(*file);
  }

  for (auto& f : filesToRemove)
    f->media()->RemoveFile(*f);

  // Insert all files at once to avoid SQL write contention
  for (auto& p : filesToAdd)
    m_ml->CreateMedia(*p, parentFolder, parentFolderFs);
  t->Commit();

  LOG_INFO("Done checking files in ", parentFolderFs.path());
}

bool mxp::FsDiscoverer::hasDotNoMediaFile(const mxp::fs::IDirectory& directory) {
  const auto& files = directory.Files();
  return std::find_if(begin(files), end(files), [](const std::shared_ptr<mxp::fs::IFile>& file) {
    return file->GetName() == ".nomedia";
  }) != end(files);
}

bool mxp::FsDiscoverer::addFolder(mxp::fs::IDirectory& folder, mxp::MediaFolder* parentFolder) const {
  auto deviceFs = folder.device();
  // We are creating a folder, there has to be a device containing it.
  assert(deviceFs != nullptr);
  auto device = mxp::MediaDevice::FindByUuid(m_ml, deviceFs->uuid());
  if (device == nullptr) {
    LOG_INFO("Creating new device in DB ", deviceFs->uuid());
    device = mxp::MediaDevice::Create(m_ml, deviceFs->uuid(), deviceFs->IsRemovable());
  }

  auto f = mxp::MediaFolder::Create(m_ml, folder.path(), parentFolder != nullptr ? parentFolder->Id() : 0, *device, *deviceFs);
                          
  LOG_INFO("Created new folder in DB ", folder.path());
  if (f == nullptr)
    return false;
  checkFolder(folder, *f);
  return true;
}
