#ifndef MEDIALISTMODEL_H
#define MEDIALISTMODEL_H

#include <QAbstractListModel>

#include <atomic>
#include <mutex>

#include "mediaexplorer/IMedia.h"

class MediaListModel : public QAbstractListModel {
  Q_OBJECT

public:
  enum Roles {
    Title = Qt::UserRole + 1,
    ThumbnailPath,
    Duration,
    Id,
  };

  explicit MediaListModel(mxp::IMediaExplorer& ml,  QObject *parent = 0);

  void addMedia(mxp::MediaPtr media);
  mxp::MediaPtr findMedia(qint64 mediaId);
  void updateMedia(mxp::MediaPtr media);
  bool removeMedia(int64_t media);
  void refresh();

  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  virtual QHash<int, QByteArray> roleNames() const override;

private:
  mxp::IMediaExplorer& m_ml;

  // Use an atomic int to avoid locking m_media within rowCount()
  mutable std::mutex         m_mediaMutex;
  std::atomic_int            m_rowCount;
  std::atomic_int            m_dropRow;
  std::vector<mxp::MediaPtr> m_media;
};

Q_DECLARE_METATYPE(mxp::MediaPtr)

#endif // MEDIALISTMODEL_H
