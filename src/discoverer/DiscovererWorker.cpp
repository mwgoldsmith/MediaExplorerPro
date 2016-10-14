/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "DiscovererWorker.h"
#include "MediaExplorer.h"
#include "logging/Logger.h"

mxp::DiscovererWorker::DiscovererWorker(mxp::MediaExplorerPtr ml)
  : m_run(false)
  , m_cb(ml->GetCallbacks()) {}

mxp::DiscovererWorker::~DiscovererWorker() {
  stop();
}

void mxp::DiscovererWorker::AddDiscoverer(std::unique_ptr<mxp::IDiscoverer> discoverer) {
  m_discoverers.push_back(std::move(discoverer));
}

void mxp::DiscovererWorker::stop() {
  bool running = true;
  if (m_run.compare_exchange_strong(running, false)) { {
      std::unique_lock<mxp::compat::Mutex> lock(m_mutex);
      while (m_tasks.empty() == false) {
        m_tasks.pop();
      }
    }
    m_cond.notify_all();
    m_thread.join();
  }
}

bool mxp::DiscovererWorker::Discover(const std::string& entryPoint) {
  if (entryPoint.length() == 0) {
    return false;
  }
  enqueue(entryPoint, false);
  return true;
}

void mxp::DiscovererWorker::reload() {
  enqueue("", true);
}

void mxp::DiscovererWorker::reload(const std::string& entryPoint) {
  enqueue(entryPoint, true);
}

void mxp::DiscovererWorker::enqueue(const std::string& entryPoint, bool reload) {
  std::unique_lock<mxp::compat::Mutex> lock(m_mutex);

  m_tasks.emplace(entryPoint, reload);
  if (m_thread.get_id() == mxp::compat::Thread::id{}) {
    m_run = true;
    m_thread = mxp::compat::Thread(&mxp::DiscovererWorker::run, this);
  } else if (m_tasks.size() == 1) {
    // Since we just added an element, let's not check for size == 0 :)
    m_cond.notify_all();
  }
}

void mxp::DiscovererWorker::run() {
  LOG_INFO("Entering DiscovererWorker thread");
  while(m_run == true) {
    Task task;
    {
      std::unique_lock<mxp::compat::Mutex> lock(m_mutex);
      if(m_tasks.size() == 0) {
        m_cond.wait(lock, [this]() {
          return m_tasks.size() > 0 || m_run == false;
        });
        if(m_run == false) {
          break;
        }
      }

      task = m_tasks.front();
      m_tasks.pop();
    }

    if(m_cb != nullptr) {
      m_cb->onDiscoveryStarted(task.entryPoint);
    }

    if(task.reload == false) {
      for(auto& d : m_discoverers) {
        // Assume only one discoverer can handle an entrypoint.
        try {
          if(d->Discover(task.entryPoint) == true) {
            break;
          }
        } catch(std::exception& ex) {
          LOG_ERROR("Fatal error while discovering ", task.entryPoint, ": ", ex.what());
        }

        if(m_run == false) {
          break;
        }
      }
    } else {
      for(auto& d : m_discoverers) {
        try {
          if(task.entryPoint.empty() == true) {
            d->reload();
          } else {
            d->reload(task.entryPoint);
          }
        } catch(std::exception& ex) {
          LOG_ERROR("Fatal error while reloading: ", ex.what());
        }

        if(m_run == false) {
          break;
        }
      }
    }

    if(m_cb != nullptr) {
      m_cb->onDiscoveryCompleted(task.entryPoint);
    }
  }

  LOG_INFO("Exiting DiscovererWorker thread");
}
