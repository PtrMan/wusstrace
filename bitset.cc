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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "bitset.hh"

#define BITSET_MASK(pos) \
	( ((size_t)1) << ((pos) % BITSET_BITS) )

#define BITSET_WORD(set, pos) \
  (set[(pos) / BITSET_BITS])

void bitset_clear_bit(bitset *set, uint32_t pos) {
  assert(pos < BITSET_SIZE);

  BITSET_WORD(set, pos) &= ~BITSET_MASK(pos);
}

void bitset_set_bit(bitset *set, uint32_t pos) {
  assert(pos < BITSET_SIZE);

  BITSET_WORD(set, pos) |= BITSET_MASK(pos);
}

bool bitset_test_bit(const bitset *set, uint32_t pos) {
  assert(pos < BITSET_SIZE);

  return (BITSET_WORD(set, pos) & BITSET_MASK(pos)) != 0;
}

bitset* bitset_init() {
  bitset *b;
  b = (bitset*) malloc(BITSET_SIZE/BITSET_BITS);
  assert(b);
  memset(b, 0, BITSET_SIZE/BITSET_BITS);
  return b;
}

void bitset_free(bitset *set) {
  free(set);
}
