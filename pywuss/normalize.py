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

import struct

from constants import *
from syscall import *
from syscall_winxp import syscall_winxp_map
from debug import *
from common import decode_base64, decode_unicode

def normalize_NtSetValueKey(pre, post):
    if pre is None:
        return

    # String & word
    if pre.getArgument(4) and pre.getArgument(4).getVal().isBasic():
        argument = pre.getArgument(5)
        data = decode_base64(argument.getVal().getValue())

        typ = int(pre.getArgument(4).getVal().getValue())

        if typ in [REG_SZ, REG_EXPAND_SZ]:
            argument.getVal().setValue(decode_unicode(data))
            argument.getVal().setType("PWSTR")
        elif typ == REG_DWORD:
            data = struct.unpack("L", data)[0]
            argument.getVal().setValue(str(data))
            argument.getVal().setType("DWORD")

def normalize_NtReadFile(pre, post):
    if post is None:
        return

    # Decode base64 buffer
    data = post.getArgument(6).getVal().getValue()
    data = decode_base64(data)
    post.getArgument(6).getVal().setValue(data)

def normalize_NtWriteFile(pre, post):
    if pre is None:
        return

    # Decode base64 buffer
    data = pre.getArgument(6).getVal().getValue()
    data = decode_base64(data)
    pre.getArgument(6).getVal().setValue(data)

def normalize_NtQueryValueKey(pre, post):
    if pre is None or post is None:
        return

    # Normalize only if the syscall succeded 
    if post.getRetval().getVal().getValue() == "0":
        # get the KeyValueInformationClass
        kvic = pre.getArgument(3).getVal().getValue()
        void = post.getArgument(4).getVal().getValue()
        void = decode_base64(void)

        if kvic == '0':
            # KeyValueBasicInformation
            assert False, "TODO"
        elif kvic == '1':
            # KeyValueFullInformation
            titleindex, type, dataoffset, datalength, namelength = \
                struct.unpack("LLLLL", void[0:20])
            data = void[20:20+namelength]
            data = decode_unicode(data)

            v = StructArgument(name = "KeyValueInformation", 
                               typ = "KEY_VALUE_FULL_INFORMATION")
            v.addField(BasicArgument(name = "TitleIndex", typ = "ULONG", v = titleindex))
            v.addField(BasicArgument(name = "Type", typ = "ULONG", v = type))
            v.addField(BasicArgument(name = "DataOffset", typ = "ULONG", v = dataoffset))
            v.addField(BasicArgument(name = "DataLength", typ = "ULONG", v = datalength))
            v.addField(BasicArgument(name = "NameLength", typ = "ULONG", v = namelength))
            v.addField(BasicArgument(name = "Name", typ = "PWCHAR", v = data))

            post.getArgument(4).setVal(v)
        elif kvic == '2':
            # KeyValuePartialInformation
            titleindex, type, datalength = struct.unpack("LLL", void[0:12])
            data       = void[12:12+datalength]

            v = StructArgument(name = "KeyValueInformation", 
                               typ = "KEY_VALUE_PARTIAL_INFORMATION")
            v.addField(BasicArgument(name = "TitleIndex", typ = "ULONG", v = titleindex))
            v.addField(BasicArgument(name = "Type", typ = "ULONG", v = type))
            v.addField(BasicArgument(name = "DataLength", typ = "ULONG", v = datalength))
            # XXX we should parse the 'Type' field and update the type
            # of the "Data" field accordingly
            if type == REG_DWORD:
                data = struct.unpack("L", data)[0]

            v.addField(BasicArgument(name = "Data", typ = "PUCHAR", v = data))

            post.getArgument(4).setVal(v)
        else:
            assert False, "! Unexpected KEY_VALUE_INFORMATION_CLASS value '%s'" % kvic

def normalize_NtSetInformationFile(pre, post):
    if pre is None:
        return

    data = pre.getArgument(3).getVal().getValue()
    data = decode_base64(data)
    pre.getArgument(3).getVal().setValue(data)

def normalize_NtCreateOpenFile(pre, post):
    if pre is None:
        return

    o = pre.getArgument(3).getVal()['ObjectName']['Buffer']
    f = o.getValue()

    f = f.strip("\r\n \t\x00")
    if pre.getTrace() is not None:
        # apply longer substitutions first
        candidatevars = []
        for name, value in pre.getTrace().getEnvironmentVariable().iteritems():
            if name.lower() in ["systemdrive", "c:", "appdata", "userprofile", "tmp", "homepath", "commonprogramfiles", 
                                "programfiles", "temp", "windir", "systemroot", "commonprogramfiles"]:
                candidatevars.append((name, value))
        candidatevars.sort(cmp = lambda x, y: cmp(len(x[1]), len(y[1])), reverse = True)
        for name, value in candidatevars:
            if value in f:
                r = "%%%s%%" % name
                f = f.replace(value, "%s\\" % r)

    o.setValue(f)

normalizemap = {
    "NtQueryValueKey" : normalize_NtQueryValueKey,
    "NtSetValueKey" : normalize_NtSetValueKey,
    "NtReadFile" : normalize_NtReadFile,
    "NtWriteFile" : normalize_NtWriteFile,
    'NtSetInformationFile': normalize_NtSetInformationFile,
    'NtCreateFile': normalize_NtCreateOpenFile,
    'NtOpenFile': normalize_NtCreateOpenFile,
    }

def normalize_value(val):
    if val.getType().startswith("PUNICODE_STRING"):
        # UNICODE_STRING
        # The argument could be NULL
        if not val.isStruct() or not val.hasField("Length"):
            return
        l = int(val.getField("Length").getValue())
        if l > 0:
            assert val.hasField("Buffer")
            buf = val["Buffer"]

            try:
                data = decode_base64(buf.getValue())
                # FIXME: data should be converted to unicode -- the following line is just a workaround!!!
                data = decode_unicode(data)
                buf.setValue(data)
            except TypeError, e:
                print "! WARNING: incorrect base64 value '%s'" % data
    elif val.isStruct():
        # normalize struct's children
        for field_name in val:
            normalize_value(val[field_name])

def normalize_filename(filename, trace = None):
    filename = filename.lower().strip("\x00 \t\n\r")

    # extract filename without path
    if '\\' in filename:
        filename = filename[filename.rfind('\\')+1:].strip()

    # remove file extension
    if '.' in filename:
        filename = filename[:filename.rfind(".")].strip()

    return filename

def normalize(syscall_pre, syscall_post):
    # guess syscall name
    if syscall_pre is None:
        syscallname = syscall_post.getName()
    elif syscall_post is None:
        syscallname = syscall_pre.getName()
    else:
        if syscall_pre.getName() != syscall_post.getName():
            print "! WARNING: syscall name mismatch: (PRE #%d) %s != (POST #%d) %s" % \
                (syscall_pre.getID(), syscall_pre.getName(), syscall_post.getID(), syscall_post.getName())
        syscallname = syscall_pre.getName()

    # generic normalization
    if syscall_pre is not None:
        for arg in syscall_pre.getArgument().itervalues():
            normalize_value(arg.getVal())
    if syscall_post is not None:
        for arg in syscall_post.getArgument().itervalues():
            normalize_value(arg.getVal())

    # syscall dependent normalization
    if syscallname in normalizemap:
        normalizemap[syscallname](syscall_pre, syscall_post)
