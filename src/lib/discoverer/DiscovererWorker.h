/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include "compat/ConditionVariable.h"
#include "compat/Mutex.h"
#include "compat/Thread.h"
#include "discoverer/IDiscoverer.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {

class DiscovererWorker : public IDiscoverer {
  struct Task {
    Task() = default;

    Task(const std::string& entryPoint, bool reload)
      : entryPoint(entryPoint)
      , reload(reload) {
    }

    std::string entryPoint;
    bool        reload;
  };

public:
  explicit DiscovererWorker(MediaExplorerPtr ml);

  virtual ~DiscovererWorker();

  virtual bool Discover(const std::string& entryPoint) override;

  virtual void reload() override;

  virtual void reload(const std::string& entryPoint) override;

  void AddDiscoverer(std::unique_ptr<IDiscoverer> discoverer);

  void stop();

private:
  void enqueue(const std::string& entryPoint, bool reload);

  void run();

  std::vector<std::unique_ptr<IDiscoverer>> m_discoverers;
  std::queue<Task>                          m_tasks;
  compat::Thread                            m_thread;
  compat::Mutex                             m_mutex;
  compat::ConditionVariable                 m_cond;
  std::atomic_bool                          m_run;
  IMediaExplorerCb*                         m_cb;
};

}
