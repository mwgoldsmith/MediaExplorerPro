﻿/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "File.h"
#include "Media.h"
#include "Parser.h"
#include "mediaexplorer/IMediaExplorer.h"
#include "utils/ModificationsNotifier.h"

namespace mxp {

Parser::Parser(MediaExplorer* ml)
  : m_ml(ml)
    , m_callback(ml->GetCallbacks())
    , m_notifier(ml->GetNotifier())
    , m_opToDo(0)
    , m_opDone(0)
    , m_percent(0) {}

Parser::~Parser() {
  stop();
}

void Parser::AddService(ServicePtr service) {
  service->initialize(m_ml, this);
  m_services.push_back(std::move(service));
}

void Parser::Parse(std::shared_ptr<Media> media, std::shared_ptr<File> file) {
  if (m_services.size() == 0)
    return;
  m_services[0]->Parse(std::unique_ptr<parser::Task>(new parser::Task(media, file)));
  m_opToDo += m_services.size();
  UpdateStats();
}

void Parser::Start() {
  restore();
  for (auto& s : m_services)
    s->Start();
}

void Parser::pause() {
  for (auto& s : m_services)
    s->pause();
}

void Parser::resume() {
  for (auto& s : m_services)
    s->resume();
}

void Parser::stop() {
  for (auto& s : m_services) {
    s->signalStop();
  }
  for (auto& s : m_services) {
    s->stop();
  }
}

void Parser::restore() {
  if (m_services.empty() == true)
    return;

  static const std::string req = "SELECT * FROM " + policy::FileTable::Name
      + " WHERE parsed = 0 AND is_present = 1";
  auto files = File::FetchAll<File>(m_ml, req);

  for (auto& f : files) {
    auto m = f->media();
    Parse(m, f);
  }
}

void Parser::UpdateStats() {
  auto percent = m_opToDo > 0 ? (m_opDone * 100 / m_opToDo) : 0;
  if (percent != m_percent) {
    m_percent = percent;
    if (m_callback != nullptr) {
      m_callback->onParsingStatsUpdated(m_percent);
    }
  }
}

void Parser::done(std::unique_ptr<parser::Task> t, parser::Task::Status status) {
  ++m_opDone;

  auto serviceIdx = ++t->currentService;

  if (status == parser::Task::Status::TemporaryUnavailable ||
    status == parser::Task::Status::Fatal) {
    if (serviceIdx < m_services.size()) {
      // We won't process the next tasks, so we need to keep the number of "todo" operations coherent:
      m_opToDo -= m_services.size() - serviceIdx;
      UpdateStats();
    }
    return;
  }
  UpdateStats();

  if (status == parser::Task::Status::Success) {
    m_notifier->NotifyMediaModification(t->media);
  }

  if (serviceIdx == m_services.size()) {
    t->file->markParsed();
    return;
  }
  m_services[serviceIdx]->Parse(std::move(t));
}

}