/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "MediaFile.h"
#include "Media.h"
#include "Parser.h"
#include "utils/ModificationsNotifier.h"

mxp::Parser::Parser(mxp::MediaExplorer* ml)
  : m_ml(ml)
  , m_callback(ml->GetCallbacks())
  , m_notifier(ml->GetNotifier())
  , m_opToDo(0)
  , m_opDone(0)
  , m_percent(0) {}

mxp::Parser::~Parser() {
  Stop();
}

void mxp::Parser::AddService(ServicePtr service) {
  service->Initialize(m_ml, this);
  m_services.push_back(std::move(service));
}

void mxp::Parser::Parse(std::shared_ptr<Media> media, std::shared_ptr<MediaFile> file) {
  if (m_services.size() == 0)
    return;
  m_services[0]->Parse(std::make_unique<parser::Task>(media, file));
  m_opToDo += m_services.size();
  UpdateStats();
}

void mxp::Parser::Start() {
  Restore();
  for (auto& s : m_services)
    s->Start();
}

void mxp::Parser::Pause() {
  for (auto& s : m_services)
    s->Pause();
}

void mxp::Parser::Resume() {
  for (auto& s : m_services)
    s->Resume();
}

void mxp::Parser::Stop() {
  for (auto& s : m_services) {
    s->SignalStop();
  }
  for (auto& s : m_services) {
    s->Stop();
  }
}

void mxp::Parser::Restore() {
  if (m_services.empty() == true)
    return;

  static const auto req = "SELECT * FROM " + policy::MediaFileTable::Name + " WHERE services_parsed_count!=" + std::to_string(m_services.size()) + " AND is_present = 1";
  auto files = MediaFile::FetchAll<MediaFile>(m_ml, req);

  for (auto& f : files) {
    auto m = f->GetMedia();
    Parse(m, f);
  }
}

void mxp::Parser::UpdateStats() {
  auto percent = m_opToDo > 0 ? static_cast<unsigned int>(m_opDone * 100 / m_opToDo) : 0;
  if (percent != m_percent) {
    m_percent = percent;
    if (m_callback != nullptr) {
      size_t done = m_opDone;
      size_t todo = m_opToDo;
      m_callback->OnParsingStatsUpdated(done, todo);
    }
  }
}

void mxp::Parser::Done(std::unique_ptr<mxp::parser::Task> t, mxp::parser::Task::Status status) {
  ++m_opDone;

  auto serviceIdx = ++t->CurrentService;

  if (status == parser::Task::Status::TemporaryUnavailable || status == parser::Task::Status::Fatal) {
    if (serviceIdx < m_services.size()) {
      // We won't process the next tasks, so we need to keep the number of "todo" operations coherent:
      m_opToDo -= m_services.size() - static_cast<size_t>(serviceIdx);;
      UpdateStats();
    }
    return;
  }

  UpdateStats();

  if (status == parser::Task::Status::Success) {
    m_notifier->NotifyMediaModification(t->Media);
  }

  t->MediaFile->SetServicesParsedCount(serviceIdx);
  if (serviceIdx == m_services.size()) {
    return;
  }

  m_services[serviceIdx]->Parse(std::move(t));
}
