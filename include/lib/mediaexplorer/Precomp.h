/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_PRECOMP_H
#define MXP_PRECOMP_H

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <cinttypes>
#include <codecvt>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <functional>
#include <fstream>
#include <iostream>
#include <locale>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <time.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#else
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <system_error>
#endif

#endif /* MXP_PRECOMP_H */
