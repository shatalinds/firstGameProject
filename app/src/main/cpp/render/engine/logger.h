//
// Created by dave on 30.04.2026.
//

#pragma once

#include <cstring>
#include <android/log.h>

#define FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, FILENAME, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, FILENAME, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, FILENAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, FILENAME, __VA_ARGS__)