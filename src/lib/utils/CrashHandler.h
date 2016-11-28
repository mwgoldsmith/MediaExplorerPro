/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef UTILS_CRASHHANDLER_H
#define UTILS_CRASHHANDLER_H

namespace mxp {
namespace utils {

class CrashHandler {
  static int s_fatalSignals[];

  CrashHandler() = delete;

public:
  static void Install();
  static void Uninstall();
};

} /* namespace utils */
} /* namespace mxp */

#endif /* UTILS_CRASHHANDLER_H */
