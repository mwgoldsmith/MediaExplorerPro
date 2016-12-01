#include "Gui/mainwindow.h"
#include "Settings/settings.h"
#include "core.h"

#include <QApplication>
#ifdef QT_STATIC
#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setApplicationName("mxpgui");
  app.setApplicationDisplayName("MediaExplorer Pro");
  app.setOrganizationName("Generisoft");

  MainWindow win;
  win.show();

  Core::getInstance()->initializeAsync();

  auto res = app.exec();
  Core::getInstance()->getSettings()->save();

  return res;
}
