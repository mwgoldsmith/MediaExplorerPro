#ifndef CORE_H
#define CORE_H

#include "Gui/workflows/libraryworkflow.h"
#include "Tools/singleton.h"

#include <QObject>
#include <QMutex>
#include <QRunnable>

namespace mxp {
class IMediaExplorer;
}

class LibraryWorkflow;
class Settings;

class Core : public QObject, public Singleton<Core> {
  Q_OBJECT

  static constexpr const char* s_dbFilename = "mxp.db";

  class InitializationRunnable : public QRunnable {
    void run();
  };

public:
  mxp::IMediaExplorer* getMediaExplorer() const;
  LibraryWorkflow* getLibraryWorkflow() const;
  Settings* getSettings() const;

  void initializeAsync();
  void shutdown();
  void statusMessage(const QString& msg);

signals:
  void appendStatusMessage(const QString& msg);
  void initializeDone();

private:
  Core();
  virtual ~Core();

  void triggerDone();
  void createSettings();

  mxp::IMediaExplorer* m_mxp;
  QMutex               m_mutex;
  Settings*            m_settings;
  LibraryWorkflow*     m_library;

  friend class Singleton<Core>;
};

#endif // CORE_H
