#ifndef MEDIAFOLDER_H
#define MEDIAFOLDER_H

#include "media.h"
#include "mediaexplorer/ILibraryFolder.h"

#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include <QUuid>

class MediaFolder;

typedef QVector<QSharedPointer<MediaFolder>> MediaFolderList;

class MediaFolder : public QObject {
  Q_OBJECT

public:
  MediaFolder() = default;
  MediaFolder(const mxp::LibraryFolderPtr libraryFolder, const MediaFolderList& children);

  virtual ~MediaFolder() {}

  const QUuid& GetUuid() const;
  const QString& GetName() const;
  bool IsVirtual() const;
  bool IsHidden() const;
  unsigned int GetPosition() const;

  const MediaFolderList& GetChildren() const;
  const MediaList& GetMedia();

signals:

public slots:

private:
  QUuid                 m_uuid;
  QString               m_name;
  bool                  m_isVirtual;
  bool                  m_isHidden;
  unsigned int          m_position;
  MediaFolderList       m_children;
  MediaList             m_media;
  mxp::LibraryFolderPtr m_libraryFolder;
};

Q_DECLARE_METATYPE(QSharedPointer<MediaFolder>)

#endif // MEDIAFOLDER_H
