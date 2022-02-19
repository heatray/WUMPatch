#pragma once

#define _WIN32_WINNT 0x0600 // Windows Vista
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES
#pragma warning(push)
#pragma warning(disable: 4178 4305 4309 4510 4996)
#include <windows.h>
#include <stdint.h>
#include <array>
#include <math.h>
#include <subauth.h>
#include <thread>
#include <mutex>
#include <set>
#include <map>
#include <iomanip>
#include "inireader\IniReader.h"
#include "injector\include\injector\injector.hpp"
#include "injector\include\injector\calling.hpp"
#include "injector\include\injector\hooking.hpp"
#ifdef _M_IX86
#include "injector\include\injector\assembly.hpp"
#endif
#include "injector\include\injector\utility.hpp"
// #include "hooking\Hooking.Patterns.h"
// #include "log.h"
// #include "ModuleList.hpp"
#include <filesystem>
#pragma warning(pop)

#ifndef CEXP
#define CEXP extern "C" __declspec(dllexport)
#endif

std::string format(const char* fmt, ...);

template<typename T>
std::array<uint8_t, sizeof(T)> to_bytes(const T& object)
{
    std::array<uint8_t, sizeof(T)> bytes;
    const uint8_t* begin = reinterpret_cast<const uint8_t*>(std::addressof(object));
    const uint8_t* end = begin + sizeof(T);
    std::copy(begin, end, std::begin(bytes));
    return bytes;
}

template<typename T>
T& from_bytes(const std::array<uint8_t, sizeof(T)>& bytes, T& object)
{
    static_assert(std::is_trivially_copyable<T>::value, "not a TriviallyCopyable type");
    uint8_t* begin_object = reinterpret_cast<uint8_t*>(std::addressof(object));
    std::copy(std::begin(bytes), std::end(bytes), begin_object);
    return object;
}

template<class T, class T1>
T from_bytes(const T1& bytes)
{
    static_assert(std::is_trivially_copyable<T>::value, "not a TriviallyCopyable type");
    T object;
    uint8_t* begin_object = reinterpret_cast<uint8_t*>(std::addressof(object));
    std::copy(std::begin(bytes), std::end(bytes) - (sizeof(T1) - sizeof(T)), begin_object);
    return object;
}

template <size_t n>
std::string pattern_str(const std::array<uint8_t, n> bytes)
{
    std::string result;
    for (size_t i = 0; i < n; i++)
    {
        result += format("%02X ", bytes[i]);
    }
    return result;
}

template <typename T>
std::string pattern_str(T t)
{
    return std::string((std::is_same<T, char>::value ? format("%c ", t) : format("%02X ", t)));
}

template <typename T, typename... Rest>
std::string pattern_str(T t, Rest... rest)
{
    return std::string((std::is_same<T, char>::value ? format("%c ", t) : format("%02X ", t)) + pattern_str(rest...));
}

template<size_t N>
constexpr size_t length(char const (&)[N])
{
    return N - 1;
}

template <typename T, typename V>
bool iequals(const T& s1, const V& s2)
{
    T str1(s1); T str2(s2);
    std::transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
    std::transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
    return (str1 == str2);
}

template<class T>
T GetModulePath(HMODULE hModule)
{
    static constexpr auto INITIAL_BUFFER_SIZE = MAX_PATH;
    static constexpr auto MAX_ITERATIONS = 7;
    T ret;
    auto bufferSize = INITIAL_BUFFER_SIZE;
    for (size_t iterations = 0; iterations < MAX_ITERATIONS; ++iterations)
    {
        ret.resize(bufferSize);
        size_t charsReturned = 0;
        if constexpr (std::is_same_v<T, std::string>)
            charsReturned = GetModuleFileNameA(hModule, &ret[0], bufferSize);
        else
            charsReturned = GetModuleFileNameW(hModule, &ret[0], bufferSize);
        if (charsReturned < ret.length())
        {
            ret.resize(charsReturned);
            return ret;
        }
        else
        {
            bufferSize *= 2;
        }
    }
    return T();
}

template<class T>
T GetThisModulePath()
{
    HMODULE hm = NULL;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&GetResolutionsList, &hm);
    T r = GetModulePath<T>(hm);
    if constexpr (std::is_same_v<T, std::string>)
        r = r.substr(0, r.find_last_of("/\\") + 1);
    else
        r = r.substr(0, r.find_last_of(L"/\\") + 1);
    return r;
}

template<class T>
T GetThisModuleName()
{
    HMODULE hm = NULL;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&GetResolutionsList, &hm);
    const T moduleFileName = GetModulePath<T>(hm);
    if constexpr (std::is_same_v<T, std::string>)
        return moduleFileName.substr(moduleFileName.find_last_of("/\\") + 1);
    else
        return moduleFileName.substr(moduleFileName.find_last_of(L"/\\") + 1);
}

template<class T>
T GetExeModulePath()
{
    T r = GetModulePath<T>(NULL);
    if constexpr (std::is_same_v<T, std::string>)
        r = r.substr(0, r.find_last_of("/\\") + 1);
    else
        r = r.substr(0, r.find_last_of(L"/\\") + 1);
    return r;
}

template<class T>
T GetExeModuleName()
{
    const T moduleFileName = GetModulePath<T>(NULL);
    if constexpr (std::is_same_v<T, std::string>)
        return moduleFileName.substr(moduleFileName.find_last_of("/\\") + 1);
    else
        return moduleFileName.substr(moduleFileName.find_last_of(L"/\\") + 1);
}

template<class T>
T GetCurrentDirectoryW()
{
    static constexpr auto INITIAL_BUFFER_SIZE = MAX_PATH;
    static constexpr auto MAX_ITERATIONS = 7;
    T ret;
    auto bufferSize = INITIAL_BUFFER_SIZE;
    for (size_t iterations = 0; iterations < MAX_ITERATIONS; ++iterations)
    {
        ret.resize(bufferSize);
        size_t charsReturned = 0;
        if constexpr (std::is_same_v<T, std::string>)
            charsReturned = GetCurrentDirectoryA(bufferSize, &ret[0]);
        else
            charsReturned = GetCurrentDirectoryW(bufferSize, &ret[0]);
        if (charsReturned < ret.length())
        {
            ret.resize(charsReturned);
            return ret;
        }
        else
        {
            bufferSize *= 2;
        }
    }
    return T();
}

template <typename T>
inline std::wstring int_to_hex(T val, size_t width = sizeof(T) * 2)
{
    std::wstringstream ss;
    ss << std::uppercase << std::setfill(L'0') << std::setw(width) << std::hex << (val | 0);
    return ss.str();
}

template <typename T>
inline std::string int_to_hex_str(T val, size_t width = sizeof(T) * 2)
{
    std::stringstream ss;
    ss << std::uppercase << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
    return ss.str();
}

template <typename T>
bool fileExists(T fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}
