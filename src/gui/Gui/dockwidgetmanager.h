#ifndef DOCKWIDGETMANAGER_H
#define DOCKWIDGETMANAGER_H

#include "Tools/qsingleton.h"

#include <QObject>
#include <QWidget>
#include <QDockWidget>
#include <QString>
#include <QMap>

class MainWindow;

class DockWidgetManager : public QObject, public QSingleton<DockWidgetManager> {
  Q_OBJECT
  Q_DISABLE_COPY(DockWidgetManager);

public:
  void setMainWindow(MainWindow* mainWin);
  QDockWidget* createDockedWidget(const char* qs_name, Qt::DockWidgetAreas areas, QDockWidget::DockWidgetFeature features);
  void addDockedWidget(QDockWidget *dockWidget, QWidget *containedWidget, Qt::DockWidgetArea startArea);
  void retranslateUi();

private:
  explicit DockWidgetManager(QObject *parent = 0);
  virtual ~DockWidgetManager();

  MainWindow*                     m_mainWin;
  QMap<const char*, QDockWidget*> m_dockWidgets;

  friend class QSingleton<DockWidgetManager>;
};

#endif // DOCKWIDGETMANAGER_H
