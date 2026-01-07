#pragma once

#include <QtCore/QtGlobal>

#ifndef QTUSB_API
#  ifdef QTUSB_STATIC
#    define QTUSB_API
#  else
#    ifdef QTUSB_EXPORTS
#      define QTUSB_API Q_DECL_EXPORT
#    else
#      define QTUSB_API Q_DECL_IMPORT
#    endif
#  endif
#endif