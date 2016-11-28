#include "mediafolder.h"
#include "media.h"
#include "mediaexplorer/IMedia.h"
#include "mxpgui.h"

MediaFolder::MediaFolder(const mxp::LibraryFolderPtr libraryFolder, const MediaFolderList& children)
  : m_uuid{ QUuid::createUuid() }
  , m_name{}
  , m_isVirtual{}
  , m_isHidden{}
  , m_position{}
  , m_children(children)
  , m_libraryFolder(libraryFolder) {
  if (libraryFolder != nullptr) {
    m_name = StdToQString(libraryFolder->GetName());
    m_isVirtual = libraryFolder->IsVirtual();
    m_isHidden = libraryFolder->IsHidden();
    m_position = libraryFolder->GetPosition();
  }
}

const QUuid& MediaFolder::GetUuid() const {
  return m_uuid;
}

const QString& MediaFolder::GetName() const {
  return m_name;
}

bool MediaFolder::IsVirtual() const {
  return m_isVirtual;
}

bool MediaFolder::IsHidden() const {
  return m_isHidden;
}

unsigned int MediaFolder::GetPosition() const {
  return m_position;
}

const MediaFolderList& MediaFolder::GetChildren() const {
  return m_children;
}

const MediaList& MediaFolder::GetMedia() {
  if(m_libraryFolder == nullptr)
    return m_media;
  if(!m_media.isEmpty())
    return m_media;

  auto media = m_libraryFolder->GetMedia();
  for (const auto& m : media) {
    auto qMedia = QSharedPointer<Media>::create(m);
    m_media.append(qMedia);
  }

  return m_media;
}
