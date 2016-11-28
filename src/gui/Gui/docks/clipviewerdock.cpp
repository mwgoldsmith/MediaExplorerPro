#include "clipviewerdock.h"
#include "ui_clipviewerdock.h"

ClipViewerDock::ClipViewerDock(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::ClipViewerDock) {
  ui->setupUi(this);
}

ClipViewerDock::~ClipViewerDock()
{
  delete ui;
}
