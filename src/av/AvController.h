/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef AVCONTROLLER_H
#define AVCONTROLLER_H

#include <memory>
#include <vector>
#include <functional>
#include "av/AvContainer.h"
#include "av/AvCodec.h"
#include "mediaexplorer/IContainer.h"
#include "mediaexplorer/ICodec.h"
#include "mediaexplorer/Types.h"
#include "Types.h"

namespace mxp {
namespace av {

MediaType GetMediaType(int libAvType);

std::string GetMediaTypeString(MediaType type);

class AvController {
  using ContainerVector = std::vector<ContainerPtr>;
  using CodecVector     = std::vector<CodecPtr>;

  struct ContainerType {
    const mstring Name;
    MediaType Type;
  };
  
public:
  AvController() = default;

  static bool Initialize(MediaExplorerPtr ml);

  static ContainerPtr FindContainer(const char* name);

  static CodecPtr FindCodec(const char* name, MediaType type);

private:
  /**
   *
   */
  static std::vector<AvContainer> GetAvContainers();

  /**
   *
   */
  static std::vector<AvCodec> GetAvCodecs();

  /**
   *
   */
  template<class T, class U>
  static void MarkUnsupported(const std::vector<T>& current, const std::vector<std::shared_ptr<U>>& stored, const char* name, std::function<void(const std::shared_ptr<U>&)> cb) {
    for(auto const& c : stored) {
      // Check if no longer exists
      auto exists = false;
      for(auto item : current) {
        const std::string &nameI = item.GetName();
        const std::string &nameC = c->GetName();
        exists = nameI.compare(nameC) == 0;
        if(exists) break;
      }

      if(!exists) {
        LOG_INFO(name, " is no longer supported: ", c->GetName());
        cb(c);
        c->SetSupported(false);
      }
    }
  }

  /**
   *
   */
  template<class T, class U>
  static void GetNewSupport(const std::vector<T>& current, const std::vector<std::shared_ptr<U>>& stored, const char* name, std::function<void(const T&)> cb) {
    for(auto const& c : current) {
      // Check if did not exist before
      auto existed = false;
      for(auto item : stored) {
        existed = item->GetName().compare(c.GetName()) == 0;
        if(existed) break;
      }

      if(!existed) {
        LOG_INFO("Support for ", name, " is now supported: ", c.GetName());
        cb(c);
      }
    }
  }

  static MediaExplorerPtr                 s_ml;
  static std::unique_ptr<ContainerVector> s_containers;
  static std::unique_ptr<CodecVector>     s_codecs;

  static ContainerType                    s_containerTypes[];
};

} /* namespace av */
} /* namespace mxp */

#endif /* AVCONTROLLER_H */
