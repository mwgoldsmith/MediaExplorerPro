#include "browserpanel.h"
#include "ui_browserpanel.h"

BrowserPanel::BrowserPanel(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::BrowserPanel) {
  m_ui->setupUi(this);
}

BrowserPanel::~BrowserPanel() {
  delete m_ui;
}
