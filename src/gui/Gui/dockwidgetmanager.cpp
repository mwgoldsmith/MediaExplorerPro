#include <QApplication>
#include <QMap>

#include "dockwidgetmanager.h"
#include "mainwindow.h"
#include <cassert>

DockWidgetManager::DockWidgetManager(QObject *parent)
  : QObject(parent) {
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(deleteLater()));
}

DockWidgetManager::~DockWidgetManager() {
  qDeleteAll(m_dockWidgets);
}

void DockWidgetManager::setMainWindow(MainWindow* mainWin) {
  m_mainWin = mainWin;
}

QDockWidget* DockWidgetManager::createDockedWidget(const char *qs_name, Qt::DockWidgetAreas areas, QDockWidget::DockWidgetFeature features) {
  assert(m_mainWin != NULL);
  if (m_dockWidgets.contains(qs_name))
    return nullptr;

  auto dock = new QDockWidget(tr(qs_name), m_mainWin);
  dock->setObjectName(QString("docked_") + qs_name);
  dock->setAllowedAreas(areas);
  dock->setFeatures(features);
  m_dockWidgets.insert(qs_name, dock);

  return dock;
}

void DockWidgetManager::addDockedWidget(QDockWidget *dockWidget, QWidget *containedWidget, Qt::DockWidgetArea startArea) {
  dockWidget->setWidget(containedWidget);
  m_mainWin->addDockWidget(startArea, dockWidget);
  m_mainWin->registerWidgetInWindowMenu(dockWidget);
}

void DockWidgetManager::retranslateUi() {
  QMapIterator<const char*, QDockWidget*> i(m_dockWidgets);

  while (i.hasNext()) {
    i.next();
    i.value()->setWindowTitle(tr(i.key()));
  }
}
