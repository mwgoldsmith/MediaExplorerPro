#include "clippropertiesdock.h"
#include "ui_clippropertiesdock.h"

ClipPropertiesDock::ClipPropertiesDock(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::ClipPropertiesDock) {
  m_ui->setupUi(this);
}

ClipPropertiesDock::~ClipPropertiesDock() {
  delete m_ui;
}
