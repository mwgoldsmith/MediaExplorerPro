/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include <atomic>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "compat/ConditionVariable.h"
#include "compat/Mutex.h"
#include "compat/Thread.h"
#include "discoverer/IDiscoverer.h"

namespace mxp {

class DiscovererWorker : public IDiscoverer {
public:
  DiscovererWorker(MediaExplorerPtr ml);
  virtual ~DiscovererWorker();
  void AddDiscoverer(std::unique_ptr<IDiscoverer> discoverer);
  void stop();

  virtual bool Discover(const std::string& entryPoint) override;
  virtual void reload() override;
  virtual void reload(const std::string& entryPoint) override;

private:
  void enqueue(const std::string& entryPoint, bool reload);
  void run();

private:
  struct Task {
    Task() = default;

    Task(const std::string& entryPoint, bool reload)
      : entryPoint(entryPoint), reload(reload) {}

    std::string entryPoint;
    bool reload;
  };

  compat::Thread m_thread;
  std::queue<Task> m_tasks;
  compat::Mutex m_mutex;
  compat::ConditionVariable m_cond;
  std::atomic_bool m_run;
  std::vector<std::unique_ptr<IDiscoverer>> m_discoverers;
  IMediaExplorerCb* m_cb;
};

}
