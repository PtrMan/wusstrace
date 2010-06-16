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


#include <windows.h>
#include <boost/thread/mutex.hpp>
#include <iostream>
#include <fstream>

using namespace std;

#include "debug.hh"
#include "common.hh"
#include "logger.hh"
#include "syscall.hh"
#include "console.hh"
#include "pending.hh"
#include "serialize-xml.hh"

extern Options options;

XMLLogger::XMLLogger(uint32_t pid, char *dumpfile) {
  // boost::mutex::scoped_lock lock(xmlmutex);

  process_id = pid;

  // xmlmutex = CreateMutex(NULL, FALSE, NULL);
  // assert(xmlmutex);

  debug("DUMPFILE: %s", dumpfile);

  dumpfs = new ofstream(dumpfile);
  assert(dumpfs && dumpfs->good());

  char filename[128];
  GetModuleFileName(NULL, filename, sizeof(filename));

  *dumpfs << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>" << endl
	  << "<!DOCTYPE boost_serialization>" << endl
	  << "<boost_serialization signature=\"serialization::archive\" version=\"5\">" << endl
	  << "<PID>" << GetCurrentProcessId() << "</PID>" << endl
	  << "<Filename>" << filename << "</Filename>" << endl;

  // Dump environment variables
  LPTCH  lpvEnv;
  LPTSTR lpszVariable;
  *dumpfs << "<Environment>" << endl;
  lpvEnv = GetEnvironmentStrings();
  lpszVariable = (LPTSTR) lpvEnv;
  if (lpvEnv != NULL) {
    // variables are separated by a NULL byte
    while (*lpszVariable) {
      string s(lpszVariable);
      string::size_type i;

      i = s.find('=');
      if (i == string::npos) {
	// '=' not found.. strange
	continue;
      }
      
      *dumpfs << "\t<Variable>" << endl;
      *dumpfs << "\t\t<Name>"  << s.substr(0, i) << "</Name>"  << endl;
      *dumpfs << "\t\t<Value>" << s.substr(i+1)    << "</Value>" << endl;
      *dumpfs << "\t</Variable>" << endl;
      
      lpszVariable += lstrlen(lpszVariable) + 1;
    }
    FreeEnvironmentStrings(lpvEnv);
  }
  *dumpfs << "</Environment>" << endl;

  oarchive = new SERIALIZE_OARCHIVE(*dumpfs, boost::archive::no_header);
  assert(oarchive);
}

XMLLogger::~XMLLogger() {
  // boost::mutex::scoped_lock lock(xmlmutex);

  assert(dumpfs);

  // write epilogue to dump file
  *dumpfs << "</boost_serialization>" << endl;

  // close ouput archive
  delete oarchive;

  // close & delete output stream
  dumpfs->flush();
  dumpfs->close();
  delete dumpfs;

  // CloseHandle(xmlmutex);
}

uint32_t XMLLogger::preSyscall(uint32_t tid, SyscallNo sysno, void *args, 
			       Syscall **syscall) {
  uint32_t r;

  r = SyscallTracer::preSyscall(tid, sysno, args, syscall);
  // debug("%s", (*syscall)->str().c_str());

#if 0
  // Check if we must fix-up this argument before serialization
  if (options.bFileOffset) {
    // Force absolute file offsets
    if ( (sysno == Syscall_NtReadFile || sysno == Syscall_NtWriteFile) ) {
      // ByteOffset argument of a Nt(Read|Write)File syscall
      HANDLE_WST hFile = (HANDLE_WST) (*syscall)->getArg(1)->getArg();
      LARGE_INTEGER liOffsetZero;

      liOffsetZero.HighPart = liOffsetZero.LowPart = 0;
      PLARGE_INTEGER offset = new LARGE_INTEGER;

      if (tls.get(tid)->extra_data)
	free(tls.get(tid)->extra_data);

      tls.get(tid)->extra_data = offset;

      if(SetFilePointerEx((HANDLE) hFile, liOffsetZero, offset, 
			  FILE_CURRENT))  {
	// SetFilePointer() successfull
	(*syscall)->getArg(8)->setArg(offset);
      }
    }
  }
#endif

  log(*syscall);

  return r;
}

uint32_t XMLLogger::postSyscall(uint32_t tid, Syscall *syscall, 
				uint32_t retval) {
  uint32_t r;

  r = SyscallTracer::postSyscall(tid, syscall, retval);

#if 0
  if (options.bPending) {
    // check if this system call is pending
    HANDLE_WST r;
    r = syscall->isPending();
    if (r != (HANDLE_WST) INVALID_HANDLE_VALUE) {
      pending_syscalls_add(syscall, r);
      // do not log this system call right now. We must tell to the caller that
      // this syscall object must not be deleted
      return 1;
    }
  }

  if (options.bPending) {
    if (syscall->getSysNo() == Syscall_NtWaitForSingleObject ||
	syscall->getSysNo() == Syscall_NtWaitForMultipleObjects) {
      Syscalls v;

      pending_syscalls_event(syscall, v);
      for (Syscalls::iterator it = v.begin(); it != v.end(); it++) {
	// Dump the output arguments of this delayed system call 
	log(*it);
	delete *it;
      }
    }
  }
#endif

  log(syscall);

  return r;
}


void XMLLogger::log(Syscall *s)
{
  boost::mutex::scoped_lock lock(mutex);

  try {
    OBJ_SAVE(*oarchive, *s, "Syscall");
  } catch (exception& e) {
    string ee = e.what();
    debug("EXCEPTION %s", ee.c_str());
    abort();
  } 
}

