#ifndef LIBRARYPANEL_H
#define LIBRARYPANEL_H

#include "Media/media.h"
#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class LibraryPanel;
}

class LibraryPanel : public QWidget {
  Q_OBJECT

public:
  explicit LibraryPanel(QWidget *parent = 0);
  ~LibraryPanel();

signals:
  void contentsChanged(const MediaList& items);
  void selectionChanged(const MediaList& items);

public slots:
  void setContents(const MediaList& media);
  void addClips(const MediaList& items);
  void removeClips(const MediaList& items);
  void reset();

private:
  QStandardItemModel* m_model;
  Ui::LibraryPanel*   m_ui;
};

#endif // LIBRARYPANEL_H
