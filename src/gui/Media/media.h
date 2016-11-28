#ifndef MEDIA_H
#define MEDIA_H

#include <QObject>
#include <QVector>
#include "mediaexplorer/IMedia.h"

class Media;

typedef QVector<QSharedPointer<Media>> MediaList;

class Media : public QObject {
  Q_OBJECT

public:
  explicit Media(const mxp::MediaPtr mediaPtr);

  int64_t GetId() const;

  QString GetName() const;

signals:

public slots:

private:
  mxp::MediaPtr m_media;
};

#endif // MEDIA_H
