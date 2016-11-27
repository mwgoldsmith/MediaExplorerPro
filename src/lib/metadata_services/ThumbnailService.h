#pragma once

#include "compat/Mutex.h"
#include "compat/ConditionVariable.h"
#include "parser/ParserService.h"

namespace mxp {

class ThumbnailService : public ParserService {
protected:
  virtual bool Initialize() override;
  virtual parser::Task::Status Run(parser::Task& task) override;
  virtual const char* Name() const override;
  virtual uint8_t nbThreads() const override;

private:
  compat::Mutex             m_mutex;
  compat::ConditionVariable m_cond;
};

}