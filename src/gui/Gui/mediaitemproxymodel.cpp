#include "mediaitemproxymodel.h"

#include <QDate>

MediaItemProxyModel::MediaItemProxyModel(QObject* parent)
  : QSortFilterProxyModel(parent) {

}

bool MediaItemProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
//    QVariant leftData = sourceModel()->data(left);
//    QVariant rightData = sourceModel()->data(right);

//    if (leftData.type() == QVariant::String) {
//            QString::localeAwareCompare(leftString.toString(), rightString.toString()) < 0;
//        } else {

            return true;
      // }
}

bool MediaItemProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);

    return (sourceModel()->data(index0).toString().contains(filterRegExp())
            || sourceModel()->data(index1).toString().contains(filterRegExp()))
          ;//  && dateInRange(sourceModel()->data(index2).toDate());
}
