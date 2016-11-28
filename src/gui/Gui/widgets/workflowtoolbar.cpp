#include "workflowtoolbar.h"

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QToolBar>
#include <QMenu>

WorkflowToolBar::WorkflowToolBar(QWidget *parent)
  : QToolBar (parent) {
  if (objectName().isEmpty())
      setObjectName(QStringLiteral("WorkflowToolBar"));
  resize(365, 50);
  setMinimumSize(QSize(0, 50));
  setMaximumSize(QSize(16777215, 50));
  setWindowTitle(QStringLiteral(""));
  setStyleSheet(QLatin1String("QToolBar {  height: 50px; }\nQToolButton { width: 71px; height: 31px; margin-left: 5px; margin-right: 5px; }"));
  setMovable(false);
  setAllowedAreas(Qt::TopToolBarArea);
  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  setFloatable(false);

  m_actionGroup = new QActionGroup(this);

  m_actionLibrary = new QAction(m_actionGroup);
  m_actionLibrary->setObjectName(QStringLiteral("actionLibrary"));
  m_actionLibrary->setCheckable(true);
  m_actionLibrary->setText(tr("Library"));
  connect(m_actionLibrary, SIGNAL(triggered(bool)), this, SLOT(toggleLibrary(bool)));

  m_actionDownloads = new QAction(m_actionGroup);
  m_actionDownloads->setObjectName(QStringLiteral("actionDownloads"));
  m_actionDownloads->setCheckable(true);
  m_actionDownloads->setText(tr("Downloads"));
  connect(m_actionDownloads, SIGNAL(triggered(bool)), this, SLOT(toggleDownloads(bool)));

  m_actionBrowser = new QAction(m_actionGroup);
  m_actionBrowser->setObjectName(QStringLiteral("actionBrowser"));
  m_actionBrowser->setCheckable(true);
  m_actionBrowser->setText(tr("Browser"));
  connect(m_actionBrowser, SIGNAL(triggered(bool)), this, SLOT(toggleBrowser(bool)));

  m_actionEditor = new QAction(m_actionGroup);
  m_actionEditor->setObjectName(QStringLiteral("actionEditor"));
  m_actionEditor->setCheckable(true);
  m_actionEditor->setText(tr("Editor"));
  connect(m_actionEditor, SIGNAL(triggered(bool)), this, SLOT(toggleEditor(bool)));

  addAction(m_actionLibrary);
  addAction(m_actionDownloads);
  addAction(m_actionBrowser);
  addAction(m_actionEditor);
}

WorkflowToolBar::~WorkflowToolBar() {
  //delete m_ui;
}

void WorkflowToolBar::addActionsToMenu(QMenu* menu) {
  menu->addAction(m_actionLibrary);
  menu->addAction(m_actionDownloads);
  menu->addAction(m_actionBrowser);
  menu->addAction(m_actionEditor);
}

void WorkflowToolBar::toggleLibrary(bool value) {
  if (!value)
    return;
  if (!m_actionLibrary->isChecked()) {
    m_actionLibrary->setChecked(true);
  }

  emit selectedLibrary();
}

void WorkflowToolBar::toggleDownloads(bool value) {
  if (!value)
    return;
  if (!m_actionDownloads->isChecked()) {
    m_actionDownloads->setChecked(true);
  }

  emit selectedDownloads();
}

void WorkflowToolBar::toggleBrowser(bool value) {
  if (!value)
    return;
  if (!m_actionBrowser->isChecked()) {
    m_actionBrowser->setChecked(true);
  }

  emit selectedBrowser();
}

void WorkflowToolBar::toggleEditor(bool value) {
  if (!value)
    return;
  if (!m_actionEditor->isChecked()) {
    m_actionEditor->setChecked(true);
  }

  emit selectedEditor();
}
