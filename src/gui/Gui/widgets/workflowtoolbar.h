#ifndef WORKFLOWTOOLBAR_H
#define WORKFLOWTOOLBAR_H

#include <QToolBar>
#include <QWidget>

//namespace Ui {
//class WorkflowToolBar;
//}

class WorkflowToolBar : public QToolBar {
  Q_OBJECT

public:
  explicit WorkflowToolBar(QWidget *parent = 0);
  ~WorkflowToolBar();

  void addActionsToMenu(QMenu* menu);

public slots:
  void toggleLibrary(bool value);
  void toggleDownloads(bool value);
  void toggleBrowser(bool value);
  void toggleEditor(bool value);

signals:
  void selectedLibrary();
  void selectedDownloads();
  void selectedBrowser();
  void selectedEditor();

private: 
  QAction*             m_actionLibrary;
  QAction*             m_actionDownloads;
  QAction*             m_actionBrowser;
  QAction*             m_actionEditor;
  QActionGroup*        m_actionGroup;
  //Ui::WorkflowToolBar* m_ui;
};

#endif // WORKFLOWTOOLBAR_H
