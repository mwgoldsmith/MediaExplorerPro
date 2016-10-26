/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef AVCONTROLLER_H
#define AVCONTROLLER_H

#include "av/AvContainer.h"
#include "av/AvCodec.h"
#include "av/StreamType.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {
namespace av {

/**
 *
 */
MediaType GetMediaType(int libAvType);

/**
 *
 */
std::string GetMediaTypeString(MediaType type);

class MediaController {
  using ContainerVector = std::vector<ContainerPtr>;
  using CodecVector     = std::vector<CodecPtr>;

  struct ContainerType {
    const mstring Name;
    MediaType Type;
  };
  

public:
  MediaController() = default;

  static bool Initialize(MediaExplorerPtr ml);

  static ContainerPtr FindContainer(const mstring name);

  static CodecPtr FindCodec(const mstring name, MediaType type);

  static MediaContextPtr CreateContext(const mstring name);

  static bool OpenStream(MediaContextPtr context, StreamType type, int index);

  static bool Seek(MediaContextPtr context, double position);

  static ImagePtr ReadFrame(MediaContextPtr context);

  static void CloseStream(MediaContextPtr context);


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
  static void GetSupportDiff(const std::vector<T>& v1, const std::vector<std::shared_ptr<U>>& v2, const char* name, std::function<void(const T&)> cb) {
    for(auto const& j : v1) {
      // Check if did not exist before
      auto exist = false;
      for(auto k : v2) {
        auto n = k->GetName();
        exist = n.compare(j.GetName()) == 0;
        if(exist) break;
      }

      if(!exist) {
        LOG_INFO("Support for ", name, " is has changed: ", j.GetName());
        cb(j);
      }
    }
  }

  /**
  *
  */
  template<class T, class U>
  static void GetSupportDiff(const std::vector<std::shared_ptr<T>>& v1, const std::vector<U>& v2, const char* name, std::function<void(const std::shared_ptr<T>&)> cb) {
    for(auto const& j : v1) {
      // Check if did not exist before
      auto exist = false;
      for(auto k : v2) {
        auto n = k.GetName();
        exist = n.compare(j->GetName()) == 0;
        if(exist) break;
      }

      if(!exist) {
        LOG_INFO("Support for ", name, " is has changed: ", j->GetName());
        cb(j);
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
