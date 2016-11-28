#ifndef MEDIAITEMDELEGATE_H
#define MEDIAITEMDELEGATE_H

#include "mediaexplorer/IMedia.h"

#include <QObject>
#include <QItemDelegate>

class MediaItemDelegate : public QItemDelegate {
  Q_OBJECT

public:
  explicit MediaItemDelegate(QObject *parent = 0);

  virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
  virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private slots:
    void commitAndCloseEditor();

private:
};

#endif // MEDIAITEMDELEGATE_H
