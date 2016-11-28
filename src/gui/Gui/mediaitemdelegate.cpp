#include "mediaitemdelegate.h"

MediaItemDelegate::MediaItemDelegate(QObject* parent) 
  : QItemDelegate(parent) {}

void MediaItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  QItemDelegate::paint(painter, option, index);
}

QWidget* MediaItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  return QItemDelegate::createEditor(parent, option, index);
}

void MediaItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
  QItemDelegate::setEditorData(editor, index);
}

void MediaItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
  QItemDelegate::setModelData(editor, model, index);
}

void MediaItemDelegate::commitAndCloseEditor() {

}
