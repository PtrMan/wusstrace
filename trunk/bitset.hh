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


#ifndef __BITSET_H__
#define __BITSET_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#define BITSET_SIZE 65536
#define BITSET_BITS 8

typedef uint8_t bitset;

bitset* bitset_init();
void bitset_free(bitset *set);

void bitset_clear_bit(bitset *set, uint32_t pos);
void bitset_set_bit(bitset *set, uint32_t pos);
bool bitset_test_bit(const bitset *set, uint32_t pos);

#endif
