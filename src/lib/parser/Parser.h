/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include "ParserService.h"
#include "MediaFile.h"
#include "Types.h"

namespace mxp {

// Use an interface to expose only the "done" method
class IParserCb {
public:
  virtual ~IParserCb() = default;
  virtual void Done(std::unique_ptr<parser::Task> task, parser::Task::Status status) = 0;
};

class Parser : IParserCb {
public:
  using ServicePtr = std::unique_ptr<ParserService>;

  explicit Parser(MediaExplorer* ml);
  virtual ~Parser();
  void AddService(ServicePtr service);
  void Parse(std::shared_ptr<Media> media, std::shared_ptr<MediaFile> file);
  void Start();
  void Pause();
  void Resume();
  void Stop();

private:
  // Queues all unparsed files for parsing.
  void Restore();
  void UpdateStats();
  virtual void Done(std::unique_ptr<parser::Task> task, parser::Task::Status status) override;

private:
  typedef std::vector<ServicePtr> ServiceList;

  ServiceList                           m_services;
  MediaExplorer*                        m_ml;
  IMediaExplorerCb*                     m_callback;
  std::shared_ptr<ModificationNotifier> m_notifier;
  std::atomic_size_t                    m_opToDo;
  std::atomic_size_t                    m_opDone;
  std::atomic_uint                      m_percent;
};

}
