#ifndef MEDIAITEMPROXYMODEL_H
#define MEDIAITEMPROXYMODEL_H

#include <QSortFilterProxyModel>

class MediaItemProxyModel : public QSortFilterProxyModel {
  Q_OBJECT

public:
  MediaItemProxyModel(QObject* parent = 0);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE;

private:
};

#endif // MEDIAITEMPROXYMODEL_H
