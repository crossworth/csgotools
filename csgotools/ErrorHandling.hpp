#pragma once
#include <iostream>


#ifdef CSGOTOOLS_THROW_ERRORS
#define CSGOTOOLS_ERROR_(message) do { throw message; } while(0)
#else
#define CSGOTOOLS_ERROR(message) do { std::cerr << std::string(message) << std::endl; } while(0)
#endif


#ifdef _DEBUG
#ifdef _MSC_VER
#define CSGOTOOLS_DEBUG_ASSERT(expr)  do { if (!(bool)(expr)) { __debugbreak();} } while(0)
#elif defined(__MACH__)
#define CSGOTOOLS_DEBUG_ASSERT(expr)  do { if (!(bool)(expr)) { __builtin_trap();} } while(0)
#else
#define CSGOTOOLS_DEBUG_ASSERT(expr)  do { if (!(bool)(expr)) { (*(u32*)0) = 0xDeAdBeEf;} } while(0)
#endif
#else
#define CSGOTOOLS_DEBUG_ASSERT(expr)
#endif


