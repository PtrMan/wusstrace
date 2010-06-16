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

# some Windows-specific constants

# syscalls error codes
STATUS_PENDING = 0x103

# registry data types
REG_NONE = 0
REG_SZ = 1
REG_EXPAND_SZ = 2
REG_BINARY = 3
REG_DWORD_LITTLE_ENDIAN = 4
REG_DWORD = 4
REG_DWORD_BIG_ENDIAN = 5
REG_LINK = 6
REG_MULTI_SZ = 7
REG_RESOURCE_LIST = 8
REG_FULL_RESOURCE_DESCRIPTOR = 9
REG_RESOURCE_REQUIREMENTS_LIST = 10
REG_QWORD_LITTLE_ENDIAN = 11
REG_QWORD = 11

# AFDs
AFD_BIND = 0x12003
AFD_CONNECT  = 0x12007
AFD_SELECT = 0x12024
AFD_SET_CONTEXT = 0x12047
AFD_GET_INFO = 0x1207b

# IO_STATUS_BLOCK.Information values
FILE_CREATED = 0
FILE_OPENED = 1
FILE_OVERWRITTEN = 2
FILE_SUPERSEDED = 3
FILE_EXISTS = 4
FILE_DOES_NOT_EXIST = 5

# FILE_INFORMATION_CLASS
class FILE_INFORMATION_CLASS:
    FileDirectoryInformation     =  1
    FileFullDirectoryInformation =  2
    FileBothDirectoryInformation =  3
    FileBasicInformation         =  4
    FileStandardInformation      =  5
    FileInternalInformation      =  6
    FileEaInformation            =  7
    FileAccessInformation        =  8
    FileNameInformation          =  9
    FileRenameInformation        = 10
    FileLinkInformation          = 11
    FileNamesInformation         = 12
    FileDispositionInformation   = 13
    FilePositionInformation      = 14
    # there are still other elements
