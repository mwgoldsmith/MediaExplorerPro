#include "libraryfoldersdock.h"
#include "ui_libraryfoldersdock.h"

LibraryFoldersDock::LibraryFoldersDock(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::LibraryFoldersDock) {
  m_ui->setupUi(this);
  connect(m_ui->foldersTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
}

LibraryFoldersDock::~LibraryFoldersDock() {
  delete m_ui;
}

void LibraryFoldersDock::addTreeRoot(const QSharedPointer<MediaFolder>& item) const {
  auto treeItem = new QTreeWidgetItem(m_ui->foldersTreeWidget);
  treeItem->setText(0, item->GetName());

  auto data = QVariant::fromValue(item);
  treeItem->setData(0, Qt::UserRole, data);
  if (item->GetChildren().size() > 0) {
    addTreeChild(treeItem, item->GetChildren());
  }
}

void LibraryFoldersDock::addTreeChild(QTreeWidgetItem *parent, const MediaFolderList& children) const {
  for(const auto& item : children) {
    auto treeItem = new QTreeWidgetItem();
    treeItem->setText(0, item->GetName());

    auto data = QVariant::fromValue(item);
    treeItem->setData(0, Qt::UserRole, data);
    if(item->GetChildren().size() > 0) {
      addTreeChild(treeItem, item->GetChildren());
    }

    parent->addChild(treeItem);
  }
}

void LibraryFoldersDock::itemSelectionChanged() {
  MediaFolderList folders{};

  auto items = m_ui->foldersTreeWidget->selectedItems();
  foreach(auto item, items) {
    auto data = item->data(0, Qt::UserRole);
    auto folder = data.value<QSharedPointer<MediaFolder>>();
    folders.append(folder);
  }

  emit selectionChanged(folders);
}

void LibraryFoldersDock::setContents(const MediaFolderList& items) {
  m_ui->foldersTreeWidget->reset();
  for(const auto& item : items) {
    addTreeRoot(item);
  }

  emit contentsChanged(items);
}

void LibraryFoldersDock::removeFolders(const MediaFolderList& items) {

}

void LibraryFoldersDock::reset() {
  m_ui->foldersTreeWidget->reset();
}
