# Copyright notice
# ================
# 
# Copyright (C) 2010
#     Lorenzo Martignoni <martignlo@gmail.com>
#     Roberto Paleari <roberto.paleari@gmail.com>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
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

import base64

from constants import *

def decode_base64(data):
    if not data:
        return ""
    data = data.strip()
    if not data.endswith("="):
        data += "=="
    return base64.b64decode(data)

def decode_unicode(data):
    return data[::2]

def get_value_type_name(t):
    # these are _not_ the same as their "normal" value defined in the Windows
    # header (and also in common.py)
    typemap = {
        0: "REG_NONE",
        1: "REG_SZ",
        2: "REG_EXPAND_SZ",
        3: "REG_BINARY",
        4: "REG_DWORD_LITTLE_ENDIAN",
        5: "REG_DWORD_BIG_ENDIAN",
        6: "REG_LINK", 
        7: "REG_MULTI_SZ", 
        8: "REG_RESOURCE_LIST", 
        9: "REG_FULL_RESOURCE_DESCRIPTOR", 
        10: "REG_RESOURCE_REQUIREMENTS_LIST", 
        11: "REG_QWORD_LITTLE_ENDIAN", 
        }

    if t not in typemap:
        return "unknown"
    else:
        return typemap[t]

def get_file_status_information_name(t):
    """
    Retrieve the name for the IO_STATUS_BLOCK.Information value 't', as it
    returned by a NtCreateFile or NtOpenFile system call.
    """
    typemap = {
        FILE_CREATED: "FILE_CREATED",
        FILE_OPENED : "FILE_OPENED",
        FILE_OVERWRITTEN : "FILE_OVERWRITTEN",
        FILE_SUPERSEDED : "FILE_SUPERSEDED",
        FILE_EXISTS : "FILE_EXISTS",
        FILE_DOES_NOT_EXIST : "FILE_DOES_NOT_EXIST",
        }
    
    if t not in typemap:
        return "unknown"
    else:
        return typemap[t]
