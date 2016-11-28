/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
#  include "config.h"
#endif
#include "CrashHandler.h"
#include "logging/Logger.h"

#include <signal.h>

int mxp::utils::CrashHandler::s_fatalSignals[] = {
  SIGSEGV,
  SIGABRT,
  SIGILL,
#ifdef SIGBUS
  SIGBUS,
#endif
#ifdef SIGFPE
  SIGFPE,
#endif
#ifdef SIGQUIT
  SIGQUIT,
#endif
  0
};

static void abortHandler(int sig) {
  const char* name = nullptr;
  switch (sig) {
  case SIGABRT: name = "SIGABRT";  break;
  case SIGSEGV: name = "SIGSEGV";  break;
  case SIGILL:  name = "SIGILL";   break;
#ifdef SIGBUS
  case SIGBUS:  name = "SIGBUS";   break;
#endif
#ifdef SIGFPE
  case SIGFPE:  name = "SIGFPE";   break;
#endif
#ifdef SIGQUIT
  case SIGQUIT: name = "SIGQUIT";  break;
#endif
  default: break;
  }

  signal(sig, SIG_DFL);
  
  LOG_ERROR("FATAL ERROR: ", name);
  LOG_ERROR("Aborting");

  raise(sig);
}

void mxp::utils::CrashHandler::Install() {
  void(*ohandler)(int);

  for (auto i = 0; s_fatalSignals[i]; ++i) {
    ohandler = signal(s_fatalSignals[i], abortHandler);
    if (ohandler != SIG_DFL) {
      signal(s_fatalSignals[i], ohandler);
    }
  }
}

void mxp::utils::CrashHandler::Uninstall() {
  void(*ohandler)(int);

  for (auto i = 0; s_fatalSignals[i]; ++i) {
    ohandler = signal(s_fatalSignals[i], SIG_DFL);
    if (ohandler != abortHandler) {
      signal(s_fatalSignals[i], ohandler);
    }
  }
}
