#pragma once

#include "compat/Mutex.h"
#include "compat/ConditionVariable.h"
#include "parser/ParserService.h"

namespace mxp {

class FormatService : public ParserService {
protected:
  virtual bool initialize() override;
  virtual parser::Task::Status run(parser::Task& task) override;
  virtual const char* Name() const override;
  virtual uint8_t nbThreads() const override;

  //void storeMeta(parser::Task& task, VLC::Media& vlcMedia);
  //int toInt(VLC::Media& vlcMedia, libvlc_meta_t meta, const char* name);

private:
  //VLC::Instance m_instance;
  compat::Mutex m_mutex;
  compat::ConditionVariable m_cond;
};

}