#pragma once
#include "compat/Mutex.h"

namespace mxp {
namespace av {

class MediaPlayer {
  static compat::Mutex s_lock;

public:
  ~MediaPlayer();

  bool Play();
  bool Pause();
  bool Stop();
  bool IsFullscreen();

private:
  explicit MediaPlayer(HWND hWnd);

public:
  static std::shared_ptr<MediaPlayer> Create(HWND hWnd);

private:
  HWND          m_hWnd;
  int   m_desktopWidth;
  int   m_desktopHeight;
  bool  m_isFullscreen;
};

}
}
