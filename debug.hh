// Copyright notice
// ================
//
// Copyright (C) 2010
//     Lorenzo Martignoni <martignlo@gmail.com>
//     Roberto Paleari <roberto.paleari@gmail.com>
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 3 of the License, or (at your option) any later
// version.  
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.


#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "config.hh"

#define RED "\x1b[0;31m"
#define GREEN "\x1b[0;32m"
#define YELLOW "\x1b[0;33m"
#define BLACK "\x1b[0m"

#define DEBUG_FILE stderr

#ifdef DEBUG_MODE
inline void debug_(const char *f, unsigned int l, const char *fmt, ...) {
  va_list ap;
  char tmp[1024];
  
  va_start(ap, fmt);
  vsnprintf(tmp, sizeof(tmp), fmt, ap);
  va_end(ap);
  _cprintf(tmp);
  // fprintf(DEBUG_FILE, tmp);

#ifdef DEBUG_FILE_LINE
  _cprintf(" [%s:%d]\n", f, l);
#else
  _cprintf("\n");
  // fprintf(DEBUG_FILE, "\n");
#endif
}

#define debug(...) debug_(__FILE__, __LINE__, __VA_ARGS__)

#else

#define debug(...)

#endif // defined(DEBUG_MODE)


#ifdef DEBUG_MODE
inline void debug_assert_(const char *f, unsigned int l, const char *pred, const
 char *fmt, ...) {
  va_list ap;
  
  fprintf(stderr, "ASSERTION \"%s\" [%s:%d] failed: ", pred, f, l);
  va_start(ap, fmt);
  vfprintf(DEBUG_FILE, fmt, ap);
  va_end(ap);
  fprintf(DEBUG_FILE, "\n");
}

#define debug_assert(p, ...)				\
  if (!(p)) {						\
    debug_assert_(__FILE__, __LINE__, #p, __VA_ARGS__); \
    getchar();						\
    abort();						\
  }

#else

#define debug_assert(p, ...)

#endif // defined(DEBUG_MODE)

#endif // ! defined(__DEBUG_H__)
