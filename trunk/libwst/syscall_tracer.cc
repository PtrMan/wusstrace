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

#include <assert.h>
#include "wstconfig.hh"
#include "syscall_tracer.hh"
#include "syscall.hh"

obj_peek_t obj_peek = NULL;
obj_malloc_t obj_malloc = NULL;
current_thread_id_t current_thread_id = NULL;

SyscallTracer::SyscallTracer() {
  process_id = 0;
}

SyscallTracer::SyscallTracer(uint32_t pid, obj_peek_t op, obj_malloc_t om,
	       current_thread_id_t cti) {
  process_id = pid;
  assert(obj_peek == NULL || obj_peek == op);
  obj_peek = op;
  assert(obj_malloc == NULL || obj_malloc == om);
  obj_malloc = om;
  assert(current_thread_id == NULL || current_thread_id == cti);
  current_thread_id = cti;
}

SyscallTracer::~SyscallTracer() {
  ;
}

uint32_t SyscallTracer::preSyscall(uint32_t tid, SyscallNo sysno, void *args, 
				  Syscall **syscall) {
  boost::mutex::scoped_lock lock(mutex);

  if (!tls.has(tid)) {
    tls.create(tid);
  }

  *syscall = new Syscall(sysno, args, ++(tls.get(tid)->id), tid, 
			 SyscallDir_ENTER);
  tls.get(tid)->syscall = *syscall;

  return 0;
}

uint32_t SyscallTracer::postSyscall(uint32_t tid, Syscall *syscall, 
				   uint32_t retval) {
  boost::mutex::scoped_lock lock(mutex);
  struct timeval tv;

  syscall->setReturnValue(retval);
  syscall->setDir(SyscallDir_EXIT);

  gettimeofday(&tv, NULL);
  syscall->setTimestamp(&tv);

  tls.get(tid)->syscall = NULL;

  return 0;
}
