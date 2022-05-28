#pragma once

#ifndef NDEBUG
#include <iostream>
#include <fstream>

#define DEBUG_STREAM(stream, val) stream << val << '\n';
#define DEBUG_STREAM_RANGE(stream, beg, end, sep) \
    {                                             \
        auto Beg = beg;                           \
        auto End = end;                           \
        for (; Beg != End; ++Beg)                 \
            stream << *Beg << sep;                \
    }
#define DEBUG_CONSOLE(val) DEBUG_STREAM(std::cout, val)
#define DEBUG_CONSOLE_RANGE(beg, end, sep) DEBUG_STREAM_RANGE(std::cout, beg, end, sep)

#define DEBUG_LOG(file_name, val)                       \
    {                                                   \
        std::ofstream f(#file_name, std::fstream::app); \
        DEBUG_STREAM(f, val)                            \
    }
#define DEBUG_LOG_RANGE(file_name, beg, end, sep)       \
    {                                                   \
        std::ofstream f(#file_name, std::fstream::app); \
        DEBUG_STREAM_RANGE(f, beg, end, sep)            \
    }

#else

#define DEBUG_STREAM(stream, val)
#define DEBUG_STREAM_RANGE(stream, beg, end, sep)

#define DEBUG_CONSOLE(val)
#define DEBUG_CONSOLE_RANGE(beg, end, sep)

#define DEBUG_LOG(file_name, val)
#define DEBUG_LOG_RANGE(file_name, beg, end, sep)
#endif