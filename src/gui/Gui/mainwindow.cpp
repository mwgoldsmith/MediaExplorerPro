#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Gui/widgets/workflowtoolbar.h"
#include "Docks/clipviewerdock.h"
#include "Docks/clippropertiesdock.h"
#include "Docks/statusconsoledock.h"
#include "Docks/libraryfoldersdock.h"
#include "Panels/librarypanel.h"
#include "Panels/downloadspanel.h"
#include "Panels/browserpanel.h"
#include "Panels/editorpanel.h"
#include "Media/mediafolder.h"
#include "Settings/settings.h"
#include "workflows/browserworkflow.h"
#include "workflows/downloadsworkflow.h"
#include "workflows/editorworkflow.h"
#include "workflows/libraryworkflow.h"
#include "dockwidgetmanager.h"
#include "core.h"
#include "mxpgui.h"

#include "mediaexplorer/IMediaExplorer.h"

#include <QWidget>
#include <QStackedWidget>
#include <QCloseEvent>
#include <QDockWidget>
#include <QFileDialog>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , m_ui(new Ui::MainWindow) {
  m_ui->setupUi(this);

  setWindowTitle(qApp->applicationDisplayName());
  setWindowState(Qt::WindowMaximized);

  m_workflowToolBar = new WorkflowToolBar(this);
  connect(m_workflowToolBar, SIGNAL(selectedLibrary()), this, SLOT(showLibrary()));
  connect(m_workflowToolBar, SIGNAL(selectedDownloads()), this, SLOT(showDownloader()));
  connect(m_workflowToolBar, SIGNAL(selectedBrowser()), this, SLOT(showBrowser()));
  connect(m_workflowToolBar, SIGNAL(selectedEditor()), this, SLOT(showEditor()));
  addToolBar(m_workflowToolBar);

  auto core = Core::getInstance();
  m_editorWorkflow = new EditorWorkflow(core);
  m_browserWorkflow = new BrowserWorkflow(core);
  m_downloadsWorkflow = new DownloadsWorkflow(core);
  connect(core, SIGNAL(appendStatusMessage(const QString&)), this, SLOT(appendStatusMessage(const QString&)));
  connect(core, SIGNAL(initializeDone()), core->getLibraryWorkflow(), SLOT(initialize()));
  connect(core, SIGNAL(initializeDone()), m_editorWorkflow, SLOT(initialize()));
  connect(core, SIGNAL(initializeDone()), m_browserWorkflow, SLOT(initialize()));
  connect(core, SIGNAL(initializeDone()), m_downloadsWorkflow, SLOT(initialize()));

  createMenus();
  createContextMenu();
  createDockWidgets();
  createPanels();
  connect(core->getLibraryWorkflow(), SIGNAL(foldersLoaded(const MediaFolderList&)), m_libraryFoldersDock, SLOT(setContents(const MediaFolderList&)));
  connect(m_libraryFoldersDock, SIGNAL(selectionChanged(const MediaFolderList&)), core->getLibraryWorkflow(), SLOT(loadMedia(const MediaFolderList&)));
  connect(core->getLibraryWorkflow(), SIGNAL(mediaLoaded(const MediaList&)), m_libraryPanel, SLOT(setContents(const MediaList&)));

  auto dataDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "\\";
  MXP_CREATE_PREFERENCE_PATH("mxp/ThumbnailFolderLocation", dataDir + "Thumbnails" + "\\",
                            "Thumbnail folder",
                            "The folder used to store thumbnails from media imported into the library.");
  MXP_CREATE_PREFERENCE_INT("mxp/ThumbnailWidth", 200, "Thumbnail width",
                            "The width of the generated thumbnails.");
  MXP_CREATE_PREFERENCE_INT("mxp/ThumbnailHeight", 200, "Thumbnail height",
                            "The height of the generated thumbnails.");

  Core::getInstance()->getSettings()->load();

  m_workflowToolBar->toggleLibrary(true);
  centralWidget()->setContentsMargins(0,0,0,0);
}

MainWindow::~MainWindow() {
  delete m_ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
  Core::getInstance()->shutdown();
  disconnect(this, SLOT(appendStatusMessage(const QString&)));
  event->accept();
}

void MainWindow::createActions() {
}

void MainWindow::createMenus() {
  auto fileMenu = menuBar()->addMenu(tr("&File"));
  auto fileAddMenu = fileMenu->addMenu(tr("Add"));
  addMenuAction(fileAddMenu, tr("Add File"), this, &MainWindow::addFile);
  addMenuAction(fileAddMenu, tr("Add Folder"), this, &MainWindow::addFolder);
  addMenuAction(fileMenu, tr("&Preferences"), this, &MainWindow::showPreferences);
  addMenuAction(fileMenu, tr("E&xit"), this, &MainWindow::close, nullptr, "Ctrl+Q");

  m_viewMenu = menuBar()->addMenu(tr("&View"));
  m_workflowToolBar->addActionsToMenu(m_viewMenu);
  m_viewMenu->addSeparator();
  addMenuAction(m_viewMenu, tr("Show &Search"), this, &MainWindow::showSearch);

  auto helpMenu = menuBar()->addMenu(tr("&Help"));
  addMenuAction(helpMenu, tr("&About"), this, &MainWindow::showAbout);
}

void MainWindow::createContextMenu() {

}

void MainWindow::createDockWidgets() {
  DockWidgetManager::getInstance(this)->setMainWindow(this);

  createStatusConsole();
  createClipViewerDock();
  createClipPropertiesDock();
  createLibraryFoldersDock();

  auto dock1 = qobject_cast<QDockWidget*>(m_clipViewerDock->parent());
  auto dock2 = qobject_cast<QDockWidget*>(m_clipPropertiesDock->parent());
  tabifyDockWidget(dock1, dock2);
}

void MainWindow::createStatusConsole() {
  auto docked = DockWidgetManager::getInstance()->createDockedWidget("Status Console", Qt::AllDockWidgetAreas, QDockWidget::AllDockWidgetFeatures);
  m_statusConsoleDock = new StatusConsoleDock(docked);
  DockWidgetManager::getInstance()->addDockedWidget(docked, m_statusConsoleDock, Qt::BottomDockWidgetArea);
}

void MainWindow::createClipViewerDock() {
  auto docked = DockWidgetManager::getInstance()->createDockedWidget("Clip Viewer", Qt::AllDockWidgetAreas, QDockWidget::AllDockWidgetFeatures);
  m_clipViewerDock = new ClipViewerDock(docked);
  DockWidgetManager::getInstance()->addDockedWidget(docked, m_clipViewerDock, Qt::RightDockWidgetArea);
}

void MainWindow::createClipPropertiesDock() {
  auto docked = DockWidgetManager::getInstance()->createDockedWidget("Clip Properties", Qt::AllDockWidgetAreas, QDockWidget::AllDockWidgetFeatures);
  m_clipPropertiesDock = new ClipPropertiesDock(docked);
  DockWidgetManager::getInstance()->addDockedWidget(docked, m_clipPropertiesDock, Qt::RightDockWidgetArea);
}

void MainWindow::createLibraryFoldersDock() {
  auto docked = DockWidgetManager::getInstance()->createDockedWidget("Library Folders", Qt::AllDockWidgetAreas, QDockWidget::AllDockWidgetFeatures);
  m_libraryFoldersDock = new LibraryFoldersDock(docked);
  DockWidgetManager::getInstance()->addDockedWidget(docked, m_libraryFoldersDock, Qt::LeftDockWidgetArea);
}

void MainWindow::createPanels() {
  m_panelStackedWidget = new QStackedWidget(centralWidget());
  m_panelStackedWidget->setObjectName(QStringLiteral("panelStackedWidget"));

  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(m_panelStackedWidget->sizePolicy().hasHeightForWidth());

  m_panelStackedWidget->setSizePolicy(sizePolicy);
  m_panelStackedWidget->setStyleSheet(QStringLiteral(""));

  m_libraryPanel = new LibraryPanel(m_panelStackedWidget);
  m_libraryPanel->setObjectName(QStringLiteral("libraryPanel"));
  m_panelStackedWidget->addWidget(m_libraryPanel);

  m_downloadsPanel = new DownloadsPanel(m_panelStackedWidget);
  m_downloadsPanel->setObjectName(QStringLiteral("downloadsPanel"));
  m_panelStackedWidget->addWidget(m_downloadsPanel);

  m_browserPanel = new BrowserPanel(m_panelStackedWidget);
  m_browserPanel->setObjectName(QStringLiteral("browserPanel"));
  m_panelStackedWidget->addWidget(m_browserPanel);

  m_editorPanel = new EditorPanel(m_panelStackedWidget);
  m_editorPanel->setObjectName(QStringLiteral("editorPanel"));
  m_panelStackedWidget->addWidget(m_editorPanel);

  m_ui->verticalLayout->addWidget(m_panelStackedWidget);
}

void MainWindow::appendStatusMessage(const QString& msg) const {
  m_statusConsoleDock->append(msg);
}

void MainWindow::addFile() {}

void MainWindow::addFolder() {
  auto curPath = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
  auto dir = QFileDialog::getExistingDirectory(this, tr("Add Directory"), curPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  Core::getInstance()->getMediaExplorer()->Discover(QStringToStd(dir));
}

void MainWindow::showPreferences() {
}

void MainWindow::showSearch(){
}

void MainWindow::showAbout() {
}

void MainWindow::showLibrary() {
  m_panelStackedWidget->setCurrentWidget(m_libraryPanel);
}

void MainWindow::showDownloader() {
  m_panelStackedWidget->setCurrentWidget(m_downloadsPanel);
}

void MainWindow::showBrowser() {
  m_panelStackedWidget->setCurrentWidget(m_browserPanel);
}

void MainWindow::showEditor() {
  m_panelStackedWidget->setCurrentWidget(m_editorPanel);
}

void MainWindow::registerWidgetInWindowMenu(QDockWidget* widget) {
  m_viewMenu->addAction(widget->toggleViewAction());
}
