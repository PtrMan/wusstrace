// Copyright notice
// ================
//
// Copyright (C) 2011
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

.text
.globl _syscall_hook
.extern syscall_hook_internal 	

_syscall_hook:
	movl %esp, %edx
	
	sub  $0x10, %esp
	movl %edx, (%esp)     // Original %esp (pointer to arguments)
	movl %eax, 0x4(%esp)  // Original %eax (syscall number)

	// Read original return address. %ebp could be NULL.
	cmp  $0x0, %ebp
	jne  read_retaddr
	xor  %eax, %eax
	jmp  save_retaddr
read_retaddr:
	movl 0x4(%ebp), %eax
save_retaddr:
	movl %eax, 0x8(%esp)

	// Read current thread ID (TID)
	movl %fs:(0x24), %eax
	movl %eax, 0xc(%esp)

	// The internal syscall handler will eventually call the original
	// system call, and return the retval in %eax
	call _syscall_hook_internal

	// Deallocate function arguments
	add $0x10, %esp
	
	ret
