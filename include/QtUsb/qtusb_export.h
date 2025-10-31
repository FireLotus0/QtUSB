#pragma once

#if defined(QTUSB_STATIC)
#define QTUSB_API
#else
#if defined(_WIN32) || defined(_WIN64)
#ifdef QTUSB_EXPORTS
#define QTUSB_API __declspec(dllexport)
#else
#define QTUSB_API __declspec(dllimport)
#endif
#else
#define QTUSB_API __attribute__((visibility("default")))
#endif
#endif