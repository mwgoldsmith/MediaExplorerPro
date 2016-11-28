#include "core.h"
#include "Settings/settings.h"
#include "mediaexplorer/IMediaExplorer.h"
#include "mediaexplorer/SettingsKey.h"
#include "mxpgui.h"

#include <QThreadPool>
#include <QStandardPaths>
#include <QApplication>
#include <QDir>

void Core::InitializationRunnable::run() {
  auto core = Core::getInstance();
  auto ml = core->getMediaExplorer();
  auto settings = core->getSettings();

  core->statusMessage(QStringLiteral("Initializing..."));
  auto dataDir = QStringToStd(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "\\");

  ml->Initialize(dataDir + s_dbFilename, core->m_library);

  ml->SetSettingString(mxp::SettingsKey::ThumbnailPath, QStringToStd(settings->value("mxp/ThumbnailFolderLocation")->get().toString()));
  ml->SetSettingInt(mxp::SettingsKey::ThumbnailWidth, settings->value("mxp/ThumbnailWidth")->get().toInt());
  ml->SetSettingInt(mxp::SettingsKey::ThumbnailHeight, settings->value("mxp/ThumbnailHeight")->get().toInt());

  // Initialization complete; call triggerDone() to allow GUI initialization to continue
  // while starting up the services for discovery and parsing
  core->triggerDone();
  ml->Startup();
  core->statusMessage(QStringLiteral("Initialization complete"));
}

Core::Core() {
  m_mxp = mxp_newInstance();
  m_library = new LibraryWorkflow(*m_mxp);
  createSettings();
}

Core::~Core() {
  delete m_library;
  delete m_settings;
}

mxp::IMediaExplorer* Core::getMediaExplorer() const {
  return m_mxp;
}

LibraryWorkflow* Core::getLibraryWorkflow() const {
  return m_library;
}

Settings* Core::getSettings() const {
  return m_settings;
}

void Core::statusMessage(const QString& msg) {
  emit appendStatusMessage(msg);
}

void Core::initializeAsync() {
  auto init = new InitializationRunnable();
  QThreadPool::globalInstance()->start(init);
}

void Core::triggerDone() {
  emit initializeDone();
}

void Core::createSettings() {
  auto configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+ "\\";

  m_settings = new Settings(configDir + qApp->applicationName() + ".conf");
}

void Core::shutdown() {
  statusMessage(QStringLiteral("Shutting down..."));
  QMutexLocker locker(&m_mutex);
  if (m_mxp != nullptr) {
    m_mxp->Shutdown();
    m_mxp = nullptr;
  }
}
