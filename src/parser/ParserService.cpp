/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Parser.h"
#include "ParserService.h"

mxp::ParserService::ParserService()
  : m_ml(nullptr)
    , m_cb(nullptr)
    , m_parserCb(nullptr)
    , m_stopParser(false)
    , m_paused(false) {}

void mxp::ParserService::Start() {
  // Ensure we don't start multiple times.
  assert(m_threads.size() == 0);
  for (auto i = 0u; i < nbThreads(); ++i)
    m_threads.emplace_back(&ParserService::mainloop, this);
}

void mxp::ParserService::pause() {
  std::lock_guard<compat::Mutex> lock(m_lock);
  m_paused = true;
}

void mxp::ParserService::resume() {
  std::lock_guard<compat::Mutex> lock(m_lock);
  m_paused = false;
  m_cond.notify_all();
}

void mxp::ParserService::signalStop() {
  for (auto& t : m_threads) {
    if (t.joinable()) { {
        std::lock_guard<compat::Mutex> lock(m_lock);
        m_cond.notify_all();
        m_stopParser = true;
      }
    }
  }
}

void mxp::ParserService::stop() {
  for (auto& t : m_threads) {
    if (t.joinable())
      t.join();
  }
}

void mxp::ParserService::Parse(std::unique_ptr<mxp::parser::Task> t) {
  std::lock_guard<compat::Mutex> lock(m_lock);
  m_tasks.push(std::move(t));
  m_cond.notify_all();
}

void mxp::ParserService::initialize(mxp::MediaExplorer* ml, mxp::IParserCb* parserCb) {
  m_ml = ml;
  m_cb = ml->GetCallbacks();
  m_notifier = ml->GetNotifier();
  m_parserCb = parserCb;
  // Run the service specific initializer
  initialize();
}

uint8_t mxp::ParserService::nbNativeThreads() const {
  auto nbProcs = compat::Thread::hardware_concurrency();
  if (nbProcs == 0)
    return 1;
  return nbProcs;
}

bool mxp::ParserService::initialize() {
  return true;
}

void mxp::ParserService::mainloop() {
  // It would be unsafe to call name() at the end of this function, since
  // we might stop the thread during ParserService destruction. This implies
  // that the underlying service has been deleted already.
  std::string serviceName = name();
  LOG_INFO("Entering ParserService [", serviceName, "] thread");

  while (m_stopParser == false) {
    std::unique_ptr<parser::Task> task; {
      std::unique_lock<compat::Mutex> lock(m_lock);
      if (m_tasks.empty() == true || m_paused == true) {
        LOG_INFO("Halting ParserService mainloop");
        m_cond.wait(lock, [this]() {
                      return (m_tasks.empty() == false && m_paused == false)
                          || m_stopParser == true;
                    });
        LOG_INFO("Resuming ParserService mainloop");
        // We might have been woken up because the parser is being destroyed
        if (m_stopParser == true)
          break;
      }
      // Otherwise it's safe to assume we have at least one element.
      task = std::move(m_tasks.front());
      m_tasks.pop();
    }
    
    parser::Task::Status status;

    try {
      LOG_INFO("Executing ", serviceName, " task on ", task->file->mrl());
      status = run(*task);
      LOG_INFO("Done executing ", serviceName, " task on ", task->file->mrl());
    } catch (const std::exception& ex) {
      LOG_ERROR("Caught an exception during ", task->file->mrl(), " ", serviceName, " parsing: ", ex.what());
      status = parser::Task::Status::Fatal;
    }
    m_parserCb->done(std::move(task), status);
  }

  LOG_INFO("Exiting ParserService [", serviceName, "] thread");
}
