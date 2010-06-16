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

#ifndef _TLS_HH
#define _TLS_HH

#include "syscall.hh"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <map>
#include <boost/thread/mutex.hpp>

typedef struct {
  Syscall *syscall;
  uint32_t id;
  void *extra_data;
} thread_data_t;

typedef map<uint32_t, thread_data_t*> thread_data_map;

class ThreadLocalStorage {
private:
  // boost::mutex tlsmutex;
  thread_data_map tls_data;
  // 'id' is the global (not thread-specific) sequential syscall identifier
  uint32_t id;

public:
  ThreadLocalStorage();
  ~ThreadLocalStorage();
  thread_data_t* get(uint32_t tid);
  void create(uint32_t tid);
  bool has(uint32_t tid);
  void erase(uint32_t tid);
};

#endif	// _TLS_HH
