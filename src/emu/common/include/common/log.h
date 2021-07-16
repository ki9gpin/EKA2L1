/*
 * Copyright (c) 2018 EKA2L1 Team.
 * 
 * This file is part of EKA2L1 project 
 * (see bentokun.github.com/EKA2L1).
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#define SPDLOG_FMT_EXTERNAL
#include <spdlog/spdlog.h>

#include <common/configure.h>

#include <memory>
#include <string>

namespace eka2l1 {
    extern bool already_setup;

    enum log_class {
        #define LOGCLASS(name, short_nice_name, nice_name) name,
        #include <common/logclass.def>
        #undef LOGCLASS
        LOG_CLASS_COUNT
    };

    const char *log_class_to_string(const log_class cls);

    class log_filterings {
    private:
        spdlog::level::level_enum levels_[LOG_CLASS_COUNT];

    public:
        explicit log_filterings();
        
        bool set_minimum_level(const log_class cls, const spdlog::level::level_enum level);
        bool is_passed(const log_class cls, const spdlog::level::level_enum level);
    };

    class base_logger {
    public:
        virtual void log(const char *fmt, ...) = 0;
        virtual void clear() = 0;
    };

    /*! \brief Contains function to setup logging. */
    namespace log {
        extern std::shared_ptr<spdlog::logger> spd_logger;
        extern std::unique_ptr<log_filterings> filterings;

        /**
         * \brief Set up the logging.
         * \param extra_logger The extra logger you want to provide to the emulator.
		*/
        void setup_log(std::shared_ptr<base_logger> extra_logger);
    }
}

#ifdef DISABLE_LOGGING
#define LOG_TRACE(class, fmt, ...)
#define LOG_DEBUG(class, fmt, ...)
#define LOG_INFO(class, fmt, ...)
#define LOG_WARN(class, fmt, ...)
#define LOG_ERROR(class, fmt, ...)
#define LOG_CRITICAL(class, fmt, ...)
#define LOG_TRACE_IF(class, flag, fmt, ...)
#define LOG_DEBUG_IF(class, flag, fmt, ...)
#define LOG_INFO_IF(class, flag, fmt, ...)
#define LOG_WARN_IF(class, flag, fmt, ...)
#define LOG_ERROR_IF(class, flag, fmt, ...)
#define LOG_CRITICAL_IF(class, flag, fmt, ...)
#else
#ifdef ENABLE_SCRIPTING
#define COND_CHECK(class, serv) if (eka2l1::log::spd_logger && eka2l1::log::filterings->is_passed(class, spdlog::level::serv)) 
#define COND_CHECK_AND(class, serv) &&eka2l1::log::spd_logger && eka2l1::log::filterings->is_passed(class, spdlog::level::serv)
#else
#define COND_CHECK(class, serv) if (eka2l1::log::filterings->is_passed(class, spdlog::level::serv))
#define COND_CHECK_AND(class, serv) &&eka2l1::log::filterings->is_passed(class, spdlog::level::serv)
#endif

#define LOG_TRACE(class, fmt, ...) COND_CHECK(class, trace) eka2l1::log::spd_logger->trace("{:s}:{} [{:s}]: " fmt, __FILE__, __LINE__, log_class_to_string(class), ##__VA_ARGS__)
#define LOG_DEBUG(class, fmt, ...) COND_CHECK(class, debug) eka2l1::log::spd_logger->debug("{:s}:{} [{:s}]: " fmt, __FILE__, __LINE__, log_class_to_string(class), ##__VA_ARGS__)
#define LOG_INFO(class, fmt, ...) COND_CHECK(class, info) eka2l1::log::spd_logger->info("{:s}:{} [{:s}]: " fmt, __FILE__, __LINE__, log_class_to_string(class), ##__VA_ARGS__)
#define LOG_WARN(class, fmt, ...) COND_CHECK(class, warn) eka2l1::log::spd_logger->warn("{:s}:{} [{:s}]: " fmt, __FILE__, __LINE__, log_class_to_string(class), ##__VA_ARGS__)
#define LOG_ERROR(class, fmt, ...) COND_CHECK(class, err) eka2l1::log::spd_logger->error("{:s}:{} [{:s}]: " fmt, __FILE__, __LINE__, log_class_to_string(class), ##__VA_ARGS__)
#define LOG_CRITICAL(class, fmt, ...) COND_CHECK(class, critical) eka2l1::log::spd_logger->critical("{:s}:{} [{:s}]: " fmt, __FILE__, __LINE__, log_class_to_string(class), ##__VA_ARGS__)

#define LOG_TRACE_IF(class, flag, fmt, ...) \
    if (flag COND_CHECK_AND(class, trace))         \
    eka2l1::log::spd_logger->trace("{:s}:{} [{:s}]: " fmt, __FILE__, __LINE__, log_class_to_string(class), ##__VA_ARGS__)
#define LOG_DEBUG_IF(class, flag, fmt, ...) \
    if (flag COND_CHECK_AND(class, debug))         \
    eka2l1::log::spd_logger->debug("{:s}:{} [{:s}]: " fmt, __FILE__, __LINE__, log_class_to_string(class), ##__VA_ARGS__)
#define LOG_INFO_IF(class, flag, fmt, ...) \
    if (flag COND_CHECK_AND(class, info))        \
    eka2l1::log::spd_logger->info("{:s}:{} [{:s}]: " fmt, __FILE__, __LINE__, log_class_to_string(class), ##__VA_ARGS__)
#define LOG_WARN_IF(class, flag, fmt, ...) \
    if (flag COND_CHECK_AND(class, warn))        \
    eka2l1::log::spd_logger->warn("{:s}:{} [{:s}]: " fmt, __FILE__, __LINE__, log_class_to_string(class), ##__VA_ARGS__)
#define LOG_ERROR_IF(class, flag, fmt, ...) \
    if (flag COND_CHECK_AND(class, err))         \
    eka2l1::log::spd_logger->error("{:s}:{} [{:s}]: " fmt, __FILE__, __LINE__, log_class_to_string(class), ##__VA_ARGS__)
#define LOG_CRITICAL_IF(class, flag, fmt, ...) \
    if (flag COND_CHECK_AND(class, critical))            \
    eka2l1::log::spd_logger->critical("{:s}:{} [{:s}]: " fmt, __FILE__, __LINE__, log_class_to_string(class), ##__VA_ARGS__)
#endif
