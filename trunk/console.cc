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


#include <conio.h>
#include <stdarg.h>
#include <windows.h>
// #include <boost/thread/mutex.hpp>

#include "console.hh"

// the global pointer to the singleton console object
Console *console;

Console* Console::pinstance = 0;

Console* Console::instance(bool allocate_console)
{
  if(!pinstance) {
    // Create singleton instance
    pinstance = new Console(allocate_console);
  }

  // Return the only instance
  return pinstance;
}

Console::Console(bool allocate_console)
{
  if (allocate_console) {
    // Allocate application console
    AllocConsole();
  }
}

void Console::printmsg(const char *fmt, ...)
{
  // boost::mutex::scoped_lock lock(consolemutex);

  va_list ap;
  va_start(ap, fmt);
  _printmsg(fmt, ap);
  va_end(ap);
  _cputs("\n");
}

void Console::debugmsg(const char *f, unsigned int l, const char *fmt, ...)
{
  // boost::mutex::scoped_lock lock(consolemutex);

  va_list ap;
  va_start(ap, fmt);
  _printmsg(fmt, ap);
  va_end(ap);

#ifdef DEBUG_FILE_LINE
  _cprintf(" [%s:%d]\n", f, l);
#else
  _cputs("\n");
#endif
}

void Console::_printmsg(const char *fmt, va_list ap)
{
  char tmp[1024];

  vsnprintf(tmp, sizeof(tmp), fmt, ap);
  _cputs(tmp);
}

