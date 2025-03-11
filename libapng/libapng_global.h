#pragma once

#include <QtCore/qglobal.h>

#if defined(LIBAPNG_LIBRARY)
#define LIBAPNG_EXPORT Q_DECL_EXPORT
#else
#define LIBAPNG_EXPORT Q_DECL_IMPORT
#endif
