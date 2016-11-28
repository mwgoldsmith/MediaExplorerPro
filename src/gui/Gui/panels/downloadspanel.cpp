#include "downloadspanel.h"
#include "ui_downloadspanel.h"

DownloadsPanel::DownloadsPanel(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::DownloadsPanel) {
  m_ui->setupUi(this);
}

DownloadsPanel::~DownloadsPanel() {
  delete m_ui;
}
