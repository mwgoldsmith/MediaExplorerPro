#ifndef LIBRARYFOLDERSDOCK_H
#define LIBRARYFOLDERSDOCK_H

#include <QWidget>
#include "Media/mediafolder.h"

namespace Ui {
class LibraryFoldersDock;
}

class QTreeWidgetItem;

class LibraryFoldersDock : public QWidget {
  Q_OBJECT

public:
  explicit LibraryFoldersDock(QWidget *parent = 0);
  ~LibraryFoldersDock();

private:
  void addTreeRoot(const QSharedPointer<MediaFolder>& items) const;
  void addTreeChild(QTreeWidgetItem *parent, const MediaFolderList& children) const;

signals:
  void contentsChanged(const MediaFolderList& media);
  void selectionChanged(const MediaFolderList& folders);

private slots:
  void itemSelectionChanged();

public slots:
  void setContents(const MediaFolderList& items);
  void removeFolders(const MediaFolderList& items);
  void reset();

private:
  Ui::LibraryFoldersDock* m_ui;
};

#endif // LIBRARYFOLDERSDOCK_H
