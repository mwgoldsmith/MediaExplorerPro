#include "libraryworkflow.h"
#include "core.h"
#include "Gui/medialistmodel.h"

#include "mediaexplorer/IMediaExplorer.h"
#include "mediaexplorer/ILibraryFolder.h"

#include <QSet>

LibraryWorkflow::LibraryWorkflow(mxp::IMediaExplorer& ml, QObject *parent)
  : QObject(parent)
  , m_ml{ml}
  , m_model{} {
  m_model = new MediaListModel(m_ml, this);

}

MediaListModel* LibraryWorkflow::getModel() const {
  return m_model;
}

MediaFolderList LibraryWorkflow::getFolderChildren(const mxp::LibraryFolderPtr folder, const std::vector<mxp::LibraryFolderPtr>& folders) const {
  MediaFolderList result{};
  for (const mxp::LibraryFolderPtr item : folders) {
    if (item->Id() == folder->Id())
      continue;

    if (item->GetParentId() == folder->Id()) {
      auto children = getFolderChildren(item, folders);
      auto child = QSharedPointer<MediaFolder>::create(item, children);
      result.append(child);
    }
  }

  return result;
}

void LibraryWorkflow::loadFolders() {
  auto mxp = Core::getInstance()->getMediaExplorer();
  auto folders = mxp->LibraryFolderList(mxp::SortingCriteria::Default, false);
  MediaFolderList result{};

  for (const mxp::LibraryFolderPtr item : folders) {
    if (item->GetParentId() != 0)
      continue;

    auto children = getFolderChildren(item, folders);
    auto child = QSharedPointer<MediaFolder>::create(item, children);
    result.append(child);
  }

  emit foldersLoaded(result);
}

void LibraryWorkflow::loadMedia(const MediaFolderList& folders) {
  MediaList results{};
  QSet<int64_t> ids{};

  foreach(auto folder, folders) {
    auto media = folder->GetMedia();

    foreach(auto item, media) {
      if (!ids.contains(item->GetId())) {
        ids.insert(item->GetId());
        results.append(item);
      }
    }
  }

  emit mediaLoaded(results);
}

void LibraryWorkflow::selectDefaultFolder(const MediaFolderList& folders) {}

void LibraryWorkflow::initialize(){
  m_model->refresh();
  loadFolders();
}

void LibraryWorkflow::onMediaAdded(std::vector<mxp::MediaPtr> media) {
  for(auto const& item : media) {
    m_model->addMedia(item);
    m_model->refresh();
    auto msg = QString("Added media file: %1").arg(QString::fromStdString(item->GetTitle()));
    Core::getInstance()->statusMessage(msg);
  }
}

void LibraryWorkflow::onMediaUpdated(std::vector<mxp::MediaPtr> media) {
  for(auto const& item : media) {
    m_model->updateMedia(item);
    m_model->refresh();
    auto msg = QString("Updated media file: %1").arg(QString::fromStdString(item->GetTitle()));
    Core::getInstance()->statusMessage(msg);
  }
}

void LibraryWorkflow::onMediaDeleted(std::vector<int64_t> ids) {
  for (auto id : ids) {
    m_model->removeMedia(id);
  }
}

void LibraryWorkflow::OnParsingStatsUpdated(size_t done, size_t todo) {
  auto msg = QString("Finished parsing %1 of %2 media files").arg(done).arg(todo);
  Core::getInstance()->statusMessage(msg);
}

void LibraryWorkflow::onDiscoveryStarted(const std::string& entryPoint) {
  if (entryPoint.empty() != true) {
    auto msg = QString("Started discovery: %1").arg(QString::fromStdString(entryPoint));
    Core::getInstance()->statusMessage(msg);
  }
}

void LibraryWorkflow::onDiscoveryProgress(const std::string& entryPoint) {}

void LibraryWorkflow::onDiscoveryCompleted(const std::string& entryPoint) {
  if (entryPoint.empty() == true) {
    m_model->refresh();
  } else {
    auto msg = QString("Finished discovery: %1").arg(QString::fromStdString(entryPoint));
    Core::getInstance()->statusMessage(msg);
  }
}

void LibraryWorkflow::onArtistsAdded(std::vector<mxp::ArtistPtr> artists) {}

void LibraryWorkflow::onArtistsModified(std::vector<mxp::ArtistPtr> artist) {}

void LibraryWorkflow::onArtistsDeleted(std::vector<int64_t> ids) {}

void LibraryWorkflow::onAlbumsAdded(std::vector<mxp::AlbumPtr> albums) {}

void LibraryWorkflow::onAlbumsModified(std::vector<mxp::AlbumPtr> albums) {}

void LibraryWorkflow::onAlbumsDeleted(std::vector<int64_t> ids) {}

void LibraryWorkflow::onTracksAdded(std::vector<mxp::AlbumTrackPtr> tracks) {}

void LibraryWorkflow::onTracksDeleted(std::vector<int64_t> trackIds) {}
