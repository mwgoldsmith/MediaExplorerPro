#include "editorpanel.h"
#include "ui_editorpanel.h"

EditorPanel::EditorPanel(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::EditorPanel) {
  m_ui->setupUi(this);
}

EditorPanel::~EditorPanel() {
  delete m_ui;
}
