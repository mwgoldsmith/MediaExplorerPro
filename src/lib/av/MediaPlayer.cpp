/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "av/MediaPlayer.h"

#include <SDL.h>
#include <SDL_thread.h>
#include "logging/Logger.h"

mxp::av::MediaPlayer::~MediaPlayer() {}

bool mxp::av::MediaPlayer::Play() {
  return false;
}

bool mxp::av::MediaPlayer::Pause() {
  return false;
}

bool mxp::av::MediaPlayer::Stop() {
  return false;
}

bool mxp::av::MediaPlayer::IsFullscreen() {
  return m_isFullscreen;
}

mxp::av::MediaPlayer::MediaPlayer(HWND hWnd)
  : m_hWnd(hWnd)
  , m_desktopWidth{}
  , m_desktopHeight{}
  , m_isFullscreen{} {
}

std::shared_ptr<mxp::av::MediaPlayer> mxp::av::MediaPlayer::Create(HWND hWnd) {
  auto mp = std::make_shared<MediaPlayer>(hWnd);

  s_lock.lock();
  if (SDL_WasInit(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
    s_lock.unlock();
    return mp;
  }

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE)) {
    s_lock.unlock();
    LOG_ERROR("Failed to initialize SDL.");
    return{};
  }

  s_lock.unlock();

  /* Translate keys into unicode */
  SDL_EnableUNICODE(1);

  /* Get the desktop resolution */
  /* FIXME: SDL has a problem with virtual desktop */
  mp->m_desktopWidth = SDL_GetVideoInfo()->current_w;
  mp->m_desktopHeight = SDL_GetVideoInfo()->current_h;

  /* Set main window's size */
  int display_width;
  int display_height;
  if (mp->m_isFullscreen) {
    display_width = mp->m_desktopWidth;
    display_height = mp->m_desktopHeight;
  } else {

  }

  // TODO

  return mp;
}
