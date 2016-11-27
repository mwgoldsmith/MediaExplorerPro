/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include "Task.h"
#include "mediaexplorer/Common.h"
#include "compat/ConditionVariable.h"
#include "compat/Mutex.h"
#include "compat/Thread.h"

namespace mxp {

class IParserCb;
class ModificationNotifier;
class MediaExplorer;

class ParserService {
public:
  ParserService();
  virtual ~ParserService() = default;

  void Start();
  void Pause();
  void Resume();
  /**
   *  @brief signalStop Will trigger the threads for termination.
   *  This doesn't wait for the threads to be done, but ensure they won't
   *  queue another operation.
   *  This is useful to ask all the threads to terminate asynchronously, before
   *  waiting for them to actually stop in the stop() method.
   */
  void SignalStop();
  /**
   *  @brief stop Effectively wait the underlying threads to join.
   */
  void Stop();
  void Parse(std::unique_ptr<parser::Task> t);
  void Initialize(MediaExplorer* mediaLibrary, IParserCb* parserCb);

protected:
  uint8_t nbNativeThreads() const;
  // Can be overridden to run service dependent initializations
  virtual bool Initialize();
  virtual parser::Task::Status Run(parser::Task& task) = 0;
  virtual const char* Name() const = 0;
  virtual uint8_t nbThreads() const = 0;

private:
  // Thread(s) entry point
  void mainloop();

protected:
  MediaExplorer* m_ml;
  IMediaExplorerCb* m_cb;
  std::shared_ptr<ModificationNotifier> m_notifier;

private:
  IParserCb* m_parserCb;
  bool m_stopParser;
  bool m_paused;
  compat::ConditionVariable m_cond;
  std::queue<std::unique_ptr<parser::Task>> m_tasks;
  std::vector<compat::Thread> m_threads;
  compat::Mutex m_lock;
};

}
