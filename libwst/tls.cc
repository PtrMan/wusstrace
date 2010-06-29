// Copyright notice
// ================
//
// Copyright (C) 2010
//     Lorenzo Martignoni <martignlo@gmail.com>
//     Roberto Paleari <roberto.paleari@gmail.com>
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#include <assert.h>
#include <stdio.h>

#include "syscall.hh"
#include "tls.hh"

#define TLS_INIT_PTHREAD_DATA(ptd)    \
  {				      \
    td->syscall = NULL;		      \
    td->id = 0;			      \
    td->extra_data = NULL;	      \
  }

ThreadLocalStorage::ThreadLocalStorage() {
  ;
}

ThreadLocalStorage::~ThreadLocalStorage() {
  thread_data_map::iterator it;

  for (it = tls_data.begin(); it != tls_data.end(); it++) {
    if ((*it).second) {
      if ((*it).second->extra_data) {
	free((*it).second->extra_data);
      }
      delete (*it).second;
    }
  }
}

void ThreadLocalStorage::create(uint32_t tid) {
  // boost::mutex::scoped_lock lock(tlsmutex);
  thread_data_t *td;

  if (tls_data.find(tid) == tls_data.end()) {
    td = new thread_data_t;
  
    // Set initial values
    TLS_INIT_PTHREAD_DATA(td);

    // Insert newly created entry
    tls_data[tid] = td;

    wst_debug("Created TLS for %08x", (uint32_t) tid);
  } else {
    td = tls_data[tid];

    wst_debug("Resetting TLS for %08x", (uint32_t) tid);

    TLS_INIT_PTHREAD_DATA(td);
  }
}

thread_data_t* ThreadLocalStorage::get(uint32_t tid) {
  // boost::mutex::scoped_lock lock(tlsmutex);
  thread_data_t *td;

  assert(tls_data.find(tid) != tls_data.end());

  td = tls_data[tid];
  return td;
}

void ThreadLocalStorage::erase(uint32_t tid) {
  // boost::mutex::scoped_lock lock(tlsmutex);

  if (tls_data.find(tid) == tls_data.end()) {
    // Unexisting thread ID
    return;
  }

  tls_data.erase(tid);
}

bool ThreadLocalStorage::has(uint32_t tid) {
  // boost::mutex::scoped_lock lock(tlsmutex);

  bool res;

  res = (tls_data.find(tid) != tls_data.end());

  return res;
}
