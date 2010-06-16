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

#include "ddk/ntstatus.h"
#include "pending.hh"
#include "syscall.hh"
#include "debug.hh"

#include <map>
#include <vector>

using namespace std;

// this is a map from event handles to vectors of system calls that are pending on that event
static map<HANDLE_WST, vector<Syscall*>*> pending_syscalls;

// system call 'syscall' waits for event 'hEvent'. Add it to the list of pending syscalls
void pending_syscalls_add(Syscall *syscall, HANDLE_WST hEvent) {
  debug("[I] delaying syscall %s (0x%x) pending on event %.8x", syscall_name(syscall->getSysNo()), syscall->getSysNo(), hEvent);

  if (pending_syscalls.find(hEvent) == pending_syscalls.end()) {
    pending_syscalls[hEvent] = new vector<Syscall*>;
  }
  pending_syscalls[hEvent]->push_back(syscall);
}


// check if this system call signaled some event that triggers any pending
// system calls
void pending_syscalls_event(Syscall *syscall, Syscalls &v ) {
  // we must build a list of events signaled by this system call
  vector<HANDLE_WST> handles;
  DWORD retval;

  // some sanity checks
  if (syscall->getSysNo() != Syscall_NtWaitForSingleObject && 
      syscall->getSysNo() != Syscall_NtWaitForMultipleObjects)
    return;

  // For these system calls, NT_SUCCESS(retval) is true also when the system
  // call returns STATUS_ALERTED, STATUS_TIMEOUT, and STATUS_USER_APC; in these
  // cases, we must do nothing
  retval = (DWORD) syscall->getReturnValue()->getArg();
  if (retval == STATUS_ALERTED || retval == STATUS_TIMEOUT || retval == STATUS_USER_APC)
    return;

  switch(syscall->getSysNo()) {
  case Syscall_NtWaitForSingleObject:
    handles.push_back((HANDLE_WST) syscall->getArg(1)->getArg());
    break;
  case Syscall_NtWaitForMultipleObjects:
    if ((BOOL) syscall->getArg(3)->getArg() == 0) {
      // WaitAllObject -- All objects specified in waiting function must be signaled to continue execution
      for(ULONG i = 0; i < (ULONG) syscall->getArg(1)->getArg(); i++) {
	handles.push_back( ((HANDLE_WST*)syscall->getArg(2)->getArg())[i] );
      }
    } else {
      // WaitAnyObject -- At least one of specified objects must be signaled
      ULONG n;

      // check which event was signaled
      n = retval - STATUS_WAIT_0;
      assert(n < (ULONG) syscall->getArg(1)->getArg());
      handles.push_back(((HANDLE_WST*)syscall->getArg(2)->getArg())[n]);
    }
    break;
  default:
    break;
  }

  for (vector<HANDLE_WST>::iterator h = handles.begin(); h != handles.end(); h++) {
    if (pending_syscalls.find(*h) != pending_syscalls.end()) {
      vector<Syscall*> *ps;
      ps = pending_syscalls[*h];

      debug("[I] event %.8x triggers pending syscalls", *h);
      for (Syscalls::iterator it = ps->begin(); it != ps->end(); it++) {
	debug("\t- syscall '%s'", syscall_name((*it)->getSysNo()));
	v.push_back(*it);
      }
      delete ps;
      pending_syscalls.erase(*h);
    }
  }
}
