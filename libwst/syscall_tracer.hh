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

#ifndef __SYSCALL_TRACER_H__
#define __SYSCALL_TRACER_H__

#include <stdio.h>
#include <stdint.h>

#include <boost/thread/mutex.hpp>

#include "syscall.hh"
#include "tls.hh"

typedef uint8_t * (*obj_malloc_t)(void *, size_t);

class SyscallTracer {
protected:
  ThreadLocalStorage tls;
  uint32_t process_id;
  boost::mutex mutex;

public:
  SyscallTracer();
  SyscallTracer(uint32_t, obj_peek_t = NULL, obj_malloc_t = NULL,
	       current_thread_id_t = NULL);
  virtual ~SyscallTracer();

  virtual uint32_t preSyscall(uint32_t, SyscallNo, void *, Syscall **);
  virtual uint32_t postSyscall(uint32_t, Syscall *, uint32_t);
};

#endif // !__SYSCALL_TRACER_H__
