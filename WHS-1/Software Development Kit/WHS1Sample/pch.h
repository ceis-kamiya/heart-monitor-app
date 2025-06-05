#ifndef PCH_H
#define PCH_H

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>

#ifdef _WIN64
#pragma comment(lib, "../x64/utws.lib")
#else
#pragma comment(lib, "../x86/utws.lib")
#endif

#include "framework.h"
#include "../include/utwsapi.h"

#endif //PCH_H
