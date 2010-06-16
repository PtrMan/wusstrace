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

#include <stdint.h>
#include <assert.h>

#include <windows.h>

#include <set>
#include <map>

using namespace std;

#include "wstconfig.hh"
#include "syscall.hh"
#include "serialize_internal.hh"

map <uint32_t, Syscall*> CurrentSyscall;

#define PAGESIZE 4096
#define PAGEMASK 0xFFFFF000
static set<void *> allocated_pages;

// To keep track of how to allocate the memory for deserialization (this is
// imported and redefined by other modules)
obj_allocation_t OBJ_ALLOCATION;

// Allocate the memory to store a deserialised object. The allocator supports
// two type of allocation: (1) allocation at a fixed address, (2) allocation at
// a free address. The first allocation method is used to guarantee consistency
// between arguments if they containt pointers that point inside the buffer.
void *obj_malloc(void *a, size_t n) {
#if 0
  assert(OBJ_ALLOCATION);

  // wst_debug("[obj_malloc] %p %x", a, n);

  if (OBJ_ALLOCATION & FIXED_ALLOCATION) {
    assert(OBJ_ALLOCATION & TRACKED_ALLOCATION);

    for (DWORD i = ((DWORD ) a) & PAGEMASK; 
	 i < ((((DWORD ) a) + n) & PAGEMASK) + PAGESIZE; i += PAGESIZE) {

      if (allocated_pages.find((void *) i) == allocated_pages.end()) {
	void *r;
	MEMORY_BASIC_INFORMATION buf;

	r = (void *) VirtualQuery((void *) i, &buf, sizeof(buf));
	wst_debug("STATE:%x (%s %s)PROT:%x SIZE:%x", buf.State, buf.State & MEM_COMMIT ? "COMMIT" : "", buf.State & MEM_RESERVE ? "RESERVE" : "", buf.Protect, (uint32_t) buf.RegionSize);
	// The memory is reserved, we commit it
	if (buf.State & MEM_RESERVE) {
	  // assert(buf.Protect & PAGE_READWRITE);
	  r = VirtualAlloc((void *) i, PAGESIZE, MEM_COMMIT, PAGE_READWRITE);
	  assert(r);
	  wst_debug("[obj_malloc] Allocated reserved page (at fixed address) %p", r);
	} else if (buf.State & MEM_COMMIT) {
	  // assert(buf.Protect & PAGE_READWRITE);
	  DWORD tmp;
	  r = (void *) VirtualProtect((void *) i, PAGESIZE, PAGE_EXECUTE_READWRITE, &tmp);
	  wst_debug("[obj_malloc] Changed protection of %x from %x to %x -- %p %x", i, tmp, PAGE_EXECUTE_READWRITE, r, GetLastError());
	  wst_debug("[obj_malloc] Using page %x as is %x", i, buf.Protect);
	} else {
	  DWORD tmp;
	  r = (void *) VirtualProtect((void *) i, PAGESIZE, PAGE_EXECUTE_READWRITE, &tmp);
	  r = VirtualAlloc((void *) i, PAGESIZE, MEM_COMMIT, PAGE_READWRITE);
	  assert(r == (void *) i);
	  // debug_assert(r == (void *) i, "Need page %p (%p) instead of %p (%x)", i, a, r, GetLastError());
	  // wst_debug("[obj_malloc] Allocated page (at fixed address) %p", r);
	}

	allocated_pages.insert((void *) i);
      }
    }

    return a;
  } else {
    void *r;

    // FIXME: add garbage collection
    r = malloc(n);
    assert(r);

    // wst_debug("[obj_malloc] Allocated buffer %p-%p", r, ((char *) r) + n);

    // TLS(current_syscall)->addAllocatedBuffer((char *) r);

    return r;
  }
#endif
}
