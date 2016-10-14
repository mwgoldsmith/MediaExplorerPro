#pragma once

#include "compat/Mutex.h"
#include "compat/ConditionVariable.h"
#include "parser/ParserService.h"
#include "Metadata.h"

namespace mxp {

class FormatService : public ParserService {
protected:
  virtual bool Initialize() override;
  virtual parser::Task::Status Run(parser::Task& task) override;
  virtual const char* Name() const override;
  virtual uint8_t nbThreads() const override;

  bool UpsertMetadata(mxp::MetadataPtr m, const mxp::parser::Task::MetadataInfo& tag);

  template<class T, class U>
  void  UpdateMetadata(std::shared_ptr<T>& inst, const std::vector<mxp::MetadataPtr>& orig, const std::vector<U>& cur) {
    for(const auto &tag : cur) {
      LOG_TRACE("Metadata: ", tag.Name, "=", tag.Value);

      auto exists = false;
      for(auto const& m : orig) {
        exists = UpsertMetadata(m, tag);
        if(exists) break;
      }

      if(!exists) {
        auto item = mxp::Metadata::Create(m_ml, tag.Name, tag.Value);
        inst->AddMetadata(item->Id());
      }
    }
  }

  //void storeMeta(parser::Task& task, VLC::Media& vlcMedia);
  //int toInt(VLC::Media& vlcMedia, libvlc_meta_t meta, const char* name);

private:
  //VLC::Instance m_instance;
  compat::Mutex m_mutex;
  compat::ConditionVariable m_cond;
};

}