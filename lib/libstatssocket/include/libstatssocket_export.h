#pragma once

#if defined(WIN32) || defined(_MSC_VER)

#if defined(LIBSTATSSOCKET_EXPORT_)
#define LIBSTATSSOCKET_API __declspec(dllexport)
#else
#define LIBSTATSSOCKET_API __declspec(dllimport)
#endif  // defined(LIBSTATSSOCKET_EXPORT_)

#else  // defined(WIN32)
#if defined(LIBSTATSSOCKET_EXPORT_)
#define LIBSTATSSOCKET_API __attribute__((visibility("default")))
#else
#define LIBSTATSSOCKET_API
#endif  // defined(LIBSTATSSOCKET_EXPORT_)
#endif
