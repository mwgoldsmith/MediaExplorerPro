#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>

namespace Ui {
class MainWindow;
}

class QStackedWidget;
class QAction;
class QActionGroup;
class QPlainTextEdit;
class QDockWidget;
class QMenu;
class ClipViewerDock;
class StatusConsoleDock;
class ClipPropertiesDock;
class LibraryFoldersDock;
class LibraryPanel;
class DownloadsPanel;
class BrowserPanel;
class EditorPanel;
class WorkflowToolBar;

#define EMPTY_STR(str) (!str || !*str)

class LibraryWorkflow;
class EditorWorkflow;
class BrowserWorkflow;
class DownloadsWorkflow;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void registerWidgetInWindowMenu(QDockWidget* widget);

protected:
  void closeEvent(QCloseEvent *event);

private:
  template<typename C, typename F>
  QAction* MainWindow::addMenuAction(QMenu *menu, const QString& text, const C* obj, F member, const char *icon = NULL, const char *shortcut = NULL) {
    QAction *action = NULL;
    if(!EMPTY_STR(icon)) {
      if(!EMPTY_STR(shortcut)) {
        action = menu->addAction(QIcon(icon), text, obj, member, tr(shortcut));
      } else {
        action = menu->addAction(QIcon(icon), text, obj, member);
      }
    } else {
      if(!EMPTY_STR(shortcut)) {
        action = menu->addAction(text, obj, member, tr(shortcut));
      } else {
        action = menu->addAction(text, obj, member);
      }
    }
    return action;
  }

  void createActions();
  void createMenus();
  void createContextMenu();
  void createDockWidgets();
  void createClipViewerDock();
  void createClipPropertiesDock();
  void createLibraryFoldersDock();
  void createPanels();
  void createStatusConsole();

public slots:
  void addFile();
  void addFolder();
  void showPreferences();
  void showSearch();
  void showAbout();

private slots:
  void appendStatusMessage(const QString& msg) const;

  void showLibrary();
  void showDownloader();
  void showBrowser();
  void showEditor();

private:
  WorkflowToolBar*    m_workflowToolBar;
  EditorWorkflow*     m_editorWorkflow;
  BrowserWorkflow*    m_browserWorkflow;
  DownloadsWorkflow*  m_downloadsWorkflow;
  QPlainTextEdit*     m_statusConsoleTextEdit;
  StatusConsoleDock*  m_statusConsoleDock;
  ClipViewerDock*     m_clipViewerDock;
  ClipPropertiesDock* m_clipPropertiesDock;
  LibraryFoldersDock* m_libraryFoldersDock;
  LibraryPanel*       m_libraryPanel;
  DownloadsPanel*     m_downloadsPanel;
  BrowserPanel*       m_browserPanel;
  EditorPanel*        m_editorPanel;
  QStackedWidget*     m_panelStackedWidget;
  QMenu*              m_viewMenu;
//  QAction*            m_showLibraryAction;
//  QAction*            m_showDownloaderAction;
//  QAction*            m_showBrowserAction;
//  QAction*            m_showEditorAction;
//  QAction*            m_showStatusConsoleAction;
//  QActionGroup*       m_pagesActionGroup;
  Ui::MainWindow*     m_ui;
};

#endif // MAINWINDOW_H
