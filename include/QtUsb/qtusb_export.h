#pragma once

#if defined(QTUSB_STATIC)
#define QTUSB_API
#else
#if defined(_WIN32) || defined(_WIN64)
    #if defined(QTUSB_EXPORTS)
      #define QTUSB_API __declspec(dllexport)
    #else
      #define QTUSB_API __declspec(dllimport)
    #endif
  #else
    #if __GNUC__ >= 4
      #define QTUSB_API __attribute__((visibility("default")))
    #else
      #define QTUSB_API
    #endif
  #endif
#endif