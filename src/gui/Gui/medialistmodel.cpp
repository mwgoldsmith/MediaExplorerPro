#include "medialistmodel.h"
#include "mediaexplorer/IMediaExplorer.h"
#include "mxpgui.h"

#include <QPixmap>

MediaListModel::MediaListModel(mxp::IMediaExplorer& ml, QObject *parent)
  : QAbstractListModel(parent)
  , m_ml(ml)
  , m_rowCount(0)
  , m_dropRow(-1){
}

void MediaListModel::addMedia(mxp::MediaPtr media) {
  std::lock_guard<std::mutex> lock(m_mediaMutex);
  auto size = m_media.size();
  beginInsertRows(QModelIndex(), size, size);
  m_media.push_back(media);
  m_rowCount.fetch_add(1, std::memory_order_relaxed);
  endInsertRows();
}

mxp::MediaPtr MediaListModel::findMedia(qint64 mediaId) {
  auto it = std::find_if(begin(m_media), end(m_media), [mediaId](mxp::MediaPtr m) {
    return m->Id() == mediaId;
  });
  if (it == end(m_media))
    return nullptr;
  return *it;
}

void MediaListModel::updateMedia(mxp::MediaPtr media) {
  auto m = createIndex(media->Id(), 0);
  emit dataChanged(m, m);
}

bool MediaListModel::removeMedia(int64_t mediaId) {
  std::lock_guard<std::mutex> lock(m_mediaMutex);
  auto it = std::find_if(begin(m_media), end(m_media), [mediaId](mxp::MediaPtr m) {
    return m->Id() == mediaId;
  });
  if (it == end(m_media))
    return false;
  auto idx = it - begin(m_media);
  beginRemoveRows(QModelIndex(), idx, idx);
  m_media.erase(it);
  m_rowCount.fetch_sub(1, std::memory_order_relaxed);
  if (m_dropRow)
  endRemoveRows();
  return true;
}

int MediaListModel::rowCount(const QModelIndex& index) const {
  // A valid index is any row, which doesn't have child
  // An invalid index is the root node, which does have children
  if (index.isValid() == true)
    return 0;
  return m_rowCount.load(std::memory_order_relaxed);
}

QVariant MediaListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= m_rowCount)
    return QVariant();

  mxp::MediaPtr m; {
    std::lock_guard<std::mutex> lock(m_mediaMutex);
    m = m_media.at(static_cast<size_t>(index.row()));
  }

  switch (role) {
  case Qt::DisplayRole:
  case Roles::Title:
    return QVariant(StdToQString(m->GetTitle()));
  case Qt::DecorationRole:
    return QPixmap(StdToQString(m->Thumbnail()));
  case Roles::ThumbnailPath:
    return QVariant(StdToQString(m->Thumbnail()));
  case Roles::Duration:
    return QVariant::fromValue(m->GetDuration());
  case Roles::Id:
    return QVariant::fromValue(m->Id());
  case Qt::UserRole:
    return QVariant::fromValue(m);
  default:
    break;
  }

  return QVariant();
}

QHash<int, QByteArray> MediaListModel::roleNames() const {
  return { { Roles::Title, "title" }, { Roles::ThumbnailPath, "thumbnailPath" }, { Roles::Duration, "duration" }, { Roles::Id, "id" }
  };
}

void MediaListModel::refresh() {
  std::lock_guard<std::mutex> lock(m_mediaMutex);

  beginResetModel();
  m_media = m_ml.MediaList(mxp::SortingCriteria::Default, false);
  m_rowCount = m_media.size();
  m_dropRow = -1;
  endResetModel();
}
