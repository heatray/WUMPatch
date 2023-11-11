#pragma once

#define _WIN32_WINNT 0x05010300 // Windows XP with Service Pack 3 (SP3)
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#pragma warning(push)
#pragma warning(disable: 4996)
#include <windows.h>
#include <string.h>
#include "inireader\IniReader.h"
#include "injector\injector.hpp"
#ifdef _M_IX86
#include "injector\assembly.hpp"
#endif
#pragma warning(pop)
