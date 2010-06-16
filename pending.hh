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

// This is the module that handles pending system calls. If enabled by
// command-line, pending syscalls are serialized to XML only when the event
// they are waiting is signaled

#ifndef _PENDING_HH
#define _PENDING_HH

#include "syscall.hh"

// Add a new pending system call, that waits for event 'hEvent' to be signaled
void pending_syscalls_add(Syscall *syscall, HANDLE_WST hEvent);
void pending_syscalls_event(Syscall *syscall, Syscalls &v);

#endif	// _PENDING_HH
