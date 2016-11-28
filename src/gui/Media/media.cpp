#include "media.h"
#include "core.h"
#include "mxpgui.h"


Media::Media(const mxp::MediaPtr mediaPtr) 
  : m_media(mediaPtr) {}

int64_t Media::GetId() const {
  return m_media->Id();
}

QString Media::GetName() const {
  return StdToQString(m_media->GetTitle());
}
