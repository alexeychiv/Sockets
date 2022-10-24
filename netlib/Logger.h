#pragma once

#include <stdint.h>
#include <stdio.h>
#include <mutex>
#include <ctime>

#ifdef NET_VERBOSE
    #ifdef NET_DEBUG
        namespace Net
        {
        
            template<typename... Args>
            void debugLog(
                const int line,
                const char* fileName,
                const char* severityString,
                const char* formatMessage,
                Args... args
            )
            {
                std::time_t currentTime = std::time(0);
                std::tm* timeStamp = std::localtime(&currentTime);
                char timeStampStr[80];
                strftime(timeStampStr, 80, "%c", timeStamp);
                
                printf("%s  ", timeStampStr);
                printf("%-9s", severityString);
                printf(formatMessage, args...);
                printf(" --> %s (line %d)", fileName, line);
                printf("\n");
            }
        
        } // namespace Net
        
        #define LOGI(formatMessage, ...) ::Net::debugLog(__LINE__, __FILE__, "[INFO]", formatMessage __VA_OPT__(,) __VA_ARGS__)
        #define LOGW(formatMessage, ...) ::Net::debugLog(__LINE__, __FILE__, "[WARN]", formatMessage __VA_OPT__(,) __VA_ARGS__)
        #define LOGE(formatMessage, ...) ::Net::debugLog(__LINE__, __FILE__, "[ERROR]", formatMessage __VA_OPT__(,) __VA_ARGS__)
        #define LOGC(formatMessage, ...) ::Net::debugLog(__LINE__, __FILE__, "[CRIT]", formatMessage __VA_OPT__(,) __VA_ARGS__)
    #else
        namespace Net
        {
        
            template<typename... Args>
            void log(
                const char* severityString,
                const char* formatMessage,
                Args... args
            )
            {
                std::time_t currentTime = std::time(0);
                std::tm* timeStamp = std::localtime(&currentTime);
                char timeStampStr[80];
                strftime(timeStampStr, 80, "%c", timeStamp);
                
                printf("%s  ", timeStampStr);
                printf("%-9s", severityString);
                printf(formatMessage, args...);
                printf("\n");
            }
        
        } // namespace Net
        
        #define LOGI(formatMessage, ...) ::Net::log("[INFO]", formatMessage __VA_OPT__(,) __VA_ARGS__)
        #define LOGW(formatMessage, ...) ::Net::log("[WARN]", formatMessage __VA_OPT__(,) __VA_ARGS__)
        #define LOGE(formatMessage, ...) ::Net::log("[ERROR]", formatMessage __VA_OPT__(,) __VA_ARGS__)
        #define LOGC(formatMessage, ...) ::Net::log("[CRIT]", formatMessage __VA_OPT__(,) __VA_ARGS__)
    #endif
#else
    #define LOGI(formatMessage, ...)
    #define LOGW(formatMessage, ...)
    #define LOGE(formatMessage, ...)
    #define LOGC(formatMessage, ...)
#endif


