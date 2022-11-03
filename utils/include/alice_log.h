//
// Created by HongZh on 2022/05/10, 16:14:44
//

#ifndef ALICE_TRACER_LOG_H
#define ALICE_TRACER_LOG_H

#ifdef DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif // DEBUG
#include "spdlog/spdlog.h"

namespace AliceAPI{

// macro for log type
#define ALICE_LOG_DEBUG SPDLOG_DEBUG
#define ALICE_LOG_TRACE SPDLOG_TRACE
#define ALICE_LOG_INFO SPDLOG_INFO
#define ALICE_LOG_WARN SPDLOG_WARN

#define ALICE_DEBUG_LEVEL SPDLOG_LEVEL_DEBUG
#define ALICE_TRACE_LEVEL SPDLOG_LEVEL_TRACE
#define ALICE_INFO_LEVEL SPDLOG_LEVEL_INFO
#define ALICE_WARN_LEVEL SPDLOG_LEVEL_WARN

typedef spdlog::level::level_enum AliceLogType;

class AliceLog{
public:
    AliceLog();
    ~AliceLog();

    // void setLogPattern();
    static void setLogLevel(const AliceLogType & log_t);
    
    template<typename... Args>
    static inline void submitDebugLog(spdlog::format_string_t<Args...> fmt, Args &&... args)
    {
        spdlog::default_logger_raw()->debug(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static inline void submitWarnLog(spdlog::format_string_t<Args...> fmt, Args &&... args)
    {
        spdlog::default_logger_raw()->warn(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static inline void submitInfoLog(spdlog::format_string_t<Args...> fmt, Args &&... args)
    {
        spdlog::default_logger_raw()->info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static inline void submitTraceLog(spdlog::format_string_t<Args...> fmt, Args &&... args)
    {
        spdlog::default_logger_raw()->trace(fmt, std::forward<Args>(args)...);
    }

};

}

#endif //ALICE_TRACER_LOG_H