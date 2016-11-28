#include "librarypanel.h"
#include "ui_librarypanel.h"
#include "Gui/medialistmodel.h"
#include <QWidget>
#include <QListView>
#include "core.h"
#include "Gui/mediaitemdelegate.h"

LibraryPanel::LibraryPanel(QWidget *parent)
  : QWidget(parent)
  , m_model{}
  , m_ui(new Ui::LibraryPanel) {
  m_ui->setupUi(this);
  m_ui->listView->setModel(Core::getInstance()->getLibraryWorkflow()->getModel());
  m_ui->listView->setItemDelegate(new MediaItemDelegate());
}

LibraryPanel::~LibraryPanel() {
  delete m_ui;
}

void LibraryPanel::setContents(const MediaList& media) {

  //auto count = media.size();
  //auto index = 0;
  //m_model->setRowCount(count);
  //for(int r = 0; r<count; r++) {
  //  auto item = media.at(r);
  //  auto data = new QStandardItem(item->GetName());
  //  auto variant = QVariant::fromValue(item);
  //  data->setData(variant, Qt::UserRole);

  //  m_model->setItem(r, 0, data);
  //}

  /**
   *  create MediaList of current panel selection
   *  create selection changed flag initialized to false
   *  for each Media in the panel contents
   *    if it does not exist in the media to set
   *      if it exists in current selection
   *         set selection changed flag to true
   *      remove from contents
   *  for each Media in media to set
   *    if it does not exist in contents
   *      add to contents
   *  emit contentsChanged(const MediaList& media)
   *  emit selectionChanged(const MediaList& media)
   *   -> MediaPropertiesDock::setMedia
   *   -> MediaViewDock::setMedia
   */
}

void LibraryPanel::addClips(const MediaList& items) {

}

void LibraryPanel::removeClips(const MediaList& items) {

}

void LibraryPanel::reset() {

}
