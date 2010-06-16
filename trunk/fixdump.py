#!/usr/bin/python

# Copyright notice
# ================
# 
# Copyright (C) 2010
#     Lorenzo Martignoni <martignlo@gmail.com>
#     Roberto Paleari <roberto.paleari@gmail.com>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the License, or (at your option) any later
# version.  
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

# Fix broken trace files

import sys, os

def main():
    if len(sys.argv) != 2:
        print "! syntax: python %s <dump filename>" % sys.argv[0]
        sys.exit(0)
        
    buffer = []
    f = open(sys.argv[1], 'r')
    for l in f.xreadlines():
        buffer.append(l)
        if l.strip("\n").strip() == "</Syscall>":
            print "".join(buffer),
            del buffer[:]
    print "</boost_serialization>"

if __name__ == "__main__":
    main()
