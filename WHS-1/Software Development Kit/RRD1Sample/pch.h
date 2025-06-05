#ifndef PCH_H
#define PCH_H

#include <memory>
#include <thread>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>

#ifdef _WIN64
#pragma comment(lib, "../x64/utws.lib")
#else
#pragma comment(lib, "../x86/utws.lib")
#endif

#include "framework.h"
#include "../include/utwsapi.h"

#endif //PCH_H
