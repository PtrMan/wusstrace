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


#ifndef _CONSOLE_HH
#define _CONSOLE_HH

#include <boost/thread/mutex.hpp>
#include "config.hh"

// Logs to console (all member functions must be THREAD SAFE)
// (singleton design pattern)
class Console {
public:
  static Console* instance(bool allocate_console);
  void printmsg(const char *fmt, ...);
  void debugmsg(const char *f, unsigned int l, const char *fmt, ...);
protected:
  Console(bool allocate_console);
private:
  static Console* pinstance;
  // boost::mutex consolemutex;
  void _printmsg(const char *fmt, va_list ap);
};

// The global console instance, used for output operations to console
extern Console *console;

#ifdef DEBUG_MODE
#define debug(...) console->debugmsg(__FILE__, __LINE__, __VA_ARGS__)
#else
#define debug(...)
#endif	// defined(DEBUG_MODE)

#define print(...) console->printmsg(__VA_ARGS__)

#endif	// _CONSOLE_HH
