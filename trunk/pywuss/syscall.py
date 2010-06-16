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

# constants
SyscallDir_IN  = 1
SyscallDir_OUT = 2

SyscallArgDir_IN = 1
SyscallArgDir_OUT = 2
SyscallArgDir_INOUT = 3

SyscallArgOpt_OPTIONAL = 1
SyscallArgOpt_MANDATORY = 2

# maps type names to values TO BE IGNORED
DEP_TYPES = {"HANDLE": ('4294967295','0')}

import struct

from syscall_winxp import syscall_winxp_map
from debug import *
from common import decode_base64, decode_unicode
from visitor import visitor

class Argument(object, visitor.Visitable):
    def __init__(self, name = None, typ = None):
        self.__name = name
        self.__type = typ

    def isBasic(self):
        return False

    def isStruct(self):
        return False

    def getName(self):
        return self.__name

    def getType(self):
        return self.__type

    def setType(self, t):
        self.__type = t

    def useVal(self, name):
        abstract()

    def defineVal(self, name):
        abstract()

class BasicArgument(Argument):
    def __init__(self, name, typ, v = None):
        self.__value = v
        Argument.__init__(self, name, typ)

    def isBasic(self):
        return True

    def setValue(self, v):
        self.__value = v
    
    def getValue(self):
        return self.__value

    def __str__(self):
        s = "%s (%s) = %s" % (self.getName(), self.getType(), self.__value)
        return s

    def useVal(self, name):
        if self.getType().startswith(name):
            for t in DEP_TYPES:
                if t.startswith(name) and self.getValue() in DEP_TYPES[t]:
                    # this value has to be ignored
                    return []
            return [self.getValue()]
        else:
            return []

    def defineVal(self, name):
        if self.getType().startswith("P" + name):
            # ignore defined value if equal to NULL
            if self.getValue() == '0':
                return []
            return [self.getValue()]
        else:
            return []

class StructArgument(Argument):
    def __init__(self, name, typ, v = None):
        self.__fields = {}
        Argument.__init__(self, name, typ)

    def isStruct(self):
        return True

    def getField(self, n):
        if n is None:
            return self.__fields.keys()
        else:
            return self.__fields.get(n)

    def hasField(self, n):
        return self.__fields.has_key(n)

    def addField(self, v):
        n = v.getName()
        assert n not in self.__fields, "! Duplicated field '%s'" % n
        self.__fields[n] = v

    def __iter__(self):
	return iter(self.__fields)

    def __len__(self):
	return len(self.__fields)

    def __getitem__(self, key):
	return self.__fields[key]

    def __str__(self):
        s = "%s (%s) => [" % (self.getName(), self.getType())
        for f in self.__fields:
            s += "%s: '%s'," % (f, self.__fields[f])
        s += "]"
        return s

    def useVal(self, name):
        u = []
        for f in self.__fields.values():
            u.extend(f.useVal(name))
        return u

    def defineVal(self, name):
        d = []
        for f in self.__fields.values():
            d.extend(f.defineVal(name))
        return d

class Syscall(object, visitor.Visitable):
    def __init__(self, idz, ltime, htime, num, direction, tid, peer = None, trace = None):
        assert num in syscall_winxp_map, "! Invalid syscall number %08x (%d)" % (num, num)

        self.__id       = idz
        self.__ltime    = ltime
        self.__htime    = htime
        self.__num      = num
        self.__name     = syscall_winxp_map[num]
        self.__dir      = direction
        self.__tid      = tid
        self.__args     = {}
        self.__peer     = peer
        self.__trace    = trace
        self.__retval   = None

    def __str__(self):
        if self.__dir == SyscallDir_IN:
            d = "PRE"
        else:
            d = "POST"

        res = "#%08d %30s (%3d) -- %4s, thread: %d\n" % (self.__id, self.__name, self.__num, d, self.__tid)
        res = colorize(res, COLOR_YELLOW)
        for i in self.__args:
            res += colorize("\t%d - %s\n" % (i, self.__args[i]), COLOR_GREEN)
        if self.__retval is not None:
            res += colorize("\tretval: %s\n" % self.__retval, COLOR_RED)
        return res

    def getID(self):
        return (self.__tid, self.__id)

    def getTime(self):
        return str(self.__ltime) + ":" + str(self.__htime)
    
    def getName(self):
        return self.__name

    def getThreadID(self):
        return self.__tid

    def isPre(self):
        return (self.__dir == SyscallDir_IN)

    def isPost(self):
        return not self.isPre()

    def addArgument(self, a, num):
        self.__args[num] = a

    def getArgument(self, num = None):
        if num is None:
            return self.__args
        else:
            return self.__args.get(num)

    def setArgument(self, num, v):
        self.__args[num] = v

    def addRetval(self, a):
        self.__retval = a

    def getRetval(self):
        return self.__retval

    def setPeer(self, p):
        self.__peer = p

    def getPeer(self):
        """
        Returns the "peer" of this Syscall object, if any. The peer of a
        Syscall object s is the Syscall object with the same ID, but opposite
        direction (pre/post).
        """
        return self.__peer

    def setTrace(self, t):
        self.__trace = t

    def getTrace(self):
        return self.__trace

    def use(self):
        u = []
        for a in self.__args.values():
            if a.getDirection() in [SyscallArgDir_IN, SyscallArgDir_INOUT]:
                u.append(a)
        return u

    def define(self):
        d = []
        for a in self.__args.values():
            if a.getDirection() in [SyscallArgDir_OUT, SyscallArgDir_INOUT]:
                d.append(a)
        if self.__retval is not None:
            d.append(self.__retval)
        return d

    def useVal(self, name):
        if not self.isPre():
            return []
        else:
            u = []
            for a in self.__args.values():
                if a.getDirection() == SyscallArgDir_IN:
                    u.extend(a.useVal(name))
            return u

    def defineVal(self, name):
        if self.isPre():
            return []
        else:
            d = []
            for a in self.__args.values():
                if a.getDirection() == SyscallArgDir_OUT:
                    d.extend(a.defineVal(name))
            # however, a return values should never be an handle...
            if self.__retval is not None:
                d.extend(self.__retval.defineVal(name))
            return d

class SyscallArg(object, visitor.Visitable):
    def __init__(self, num, name, direction, optional, value = None):
        self.__num  = num        # argument number (actual parameter ordinal number)
        self.__name = name       # argument type name
        self.__dir  = direction  # argument direction (IN/OUT/INOUT)
        self.__opt  = optional   # it is an optional argument?
        self.__val  = value      # argument value

    def __str__(self):
        if self.__dir == SyscallArgDir_IN:
            d = "IN"
        elif self.__dir == SyscallArgDir_OUT:
            d = "OUT"
        else:
            d = "INOUT"

        if self.__opt:
            o = "OPTIONAL"
        else:
            o = "MANDATORY"

        res = "%20s %5s (%s) ==> %s" % (self.__name, d, o, self.__val)
        return res

    def getName(self):
        return self.__name

    def getVal(self):
        return self.__val

    def setVal(self, v):
        self.__val = v

    def getNum(self):
        return self.__num

    def isOptional(self):
        return self.__opt

    def getDirection(self):
        return self.__dir

    def useVal(self, name):
        if self.__val is not None:
            return self.__val.useVal(name)
        else:
            return []

    def defineVal(self, name):
        if self.__val is not None:
            return self.__val.defineVal(name)
        else:
            return []

class SyscallTrace(object, visitor.Visitable):
    def __init__(self, name = None, pid = 0, f = None):
        self.__name = name
        self.__pid = pid
        self.__filename = f
        self.__env = {}         # environment variables
        self.__syscalls = []
        self.__premap  = {}  # maps a syscall ID to the index of a PRE syscall object inside __syscalls
        self.__postmap = {}  # maps a syscall ID to the index of a POST syscall object inside __syscalls
        self.__usemap = {}   # maps a syscall ID to usevals
        self.__defmap = {}   # maps a syscall ID to defvals

    def __iter__(self):
        ids = [id for id in (set(self.__premap.keys()) | set(self.__postmap.keys()))]
        ids.sort()

        for id in ids:
            yield id

    def __len__(self):
        return len(self.__syscalls)

    def getName(self):
        return self.__name

    def setFilename(self, f):
        self.__filename = f

    def getFilename(self):
        return self.__filename

    def setPID(self, pid):
        self.__pid = pid

    def getPID(self):
        return self.__pid

    def addEnvironmentVariable(self, name, value):
        assert name not in self.__env
        self.__env[name] = value

    def getEnvironmentVariable(self, name = None):
        if name is None:
            return self.__env
        else:
            return self.__env.get(name)

    def append(self, s):
        """
        Appends a Syscall object to the end of the trace.
        """
        self.__syscalls.append(s)

        # update Syscall ID map and adjust pre-post links
        if s.isPre():
            this  = self.__premap
            other = self.__postmap
        else:
            this  = self.__postmap
            other = self.__premap

        assert s.getID() not in this, "! Duplicated ID '%s'" % s.getID()

        this[s.getID()] = len(self.__syscalls)-1

        if s.getID() in other:
            o = self.__syscalls[other[s.getID()]]
            s.setPeer(o)
            o.setPeer(s)

    def getSyscallByID(self, id):
        """
        Returns a pair of (PRE,POST) Syscall objects with sequential ID 'id'.
        """
        if id not in self.__premap:
            pre = None
        else:
            pre = self.__syscalls[self.__premap[id]]

        if id not in self.__postmap:
            post = None
        else:
            post = self.__syscalls[self.__postmap[id]]
        
        return (pre, post)

    def getSyscalls(self):
        """
        Returns system calls in "original" order.
        """
        return self.__syscalls

    def __contains__(self, a):
        """
        Returns True if Syscall 's' belongs to this SyscallTrace object.
        """
        if not isinstance(s, Syscall):
            return False
        return s in self.__syscalls

    def __str__(self):
        res = colorize("### Syscall trace '%s' ###\n" % self.__name, COLOR_BLUE)
        for s in self.__syscalls:
            res += str(s)
        return res


    def buildUseValMap(self):
        self.__usemap.clear()

        for i in self.__premap.values():
            s = self.__syscalls[i]
            assert s.getID() not in self.__usemap
            self.__usemap[s.getID()] = []
            for t in DEP_TYPES:
                for u in s.useVal(t):
                    if u in DEP_TYPES[t]:
                        # this value has to be ignored
                        continue
                    self.__usemap[s.getID()].append((t,u))

    def buildDefValMap(self):
        self.__defmap.clear()

        for i in self.__postmap.values():
            s = self.__syscalls[i]
            assert s.getID() not in self.__defmap
            self.__defmap[s.getID()] = []
            for t in DEP_TYPES:
                for d in s.defineVal(t):
                    # here we shouldn't care about values to be ignored
                    # we only have to consider "null" (= 0) values
                    if t.startswith("HANDLE") and d == '0':
                        continue
                    self.__defmap[s.getID()].append((t,d))

    def getUseValMap(self):
        return self.__usemap

    def getDefValMap(self):
        return self.__defmap

    def reachingDefinition(self, id, v):
        # 'v' is a pair (type_name,value)
        postids = self.__postmap.keys()
        postids.sort(reverse=True)

        for p in postids:
            if p >= id:
                # skip
                continue
            # check if (post) syscall with ID 'p' defines 'v'
            s = self.getSyscallByID(p)[1]
            assert s is not None

            if v[1] in s.defineVal(v[0]):
                return s.getID()
        return None

    def defuse(self, startid, v):
        tmp = self.forwardSlice(startid, v, True)

        # remove itself
        assert tmp[0] == startid
        return tmp[1:]

    def forwardSlice(self, startid, v, onestep = False):
        """
        forwardSlice(startid,v) calculates the forward slice for the
        instruction with ID 'startid', for variable v=(type_name,value).
        """

        #### CHECKME!!! ####

        dataws = [startid]
        slice = [startid]

        preids = self.__premap.keys()
        preids.sort()

        while len(dataws) > 0:
            # follow data dependencies
            # ------------------------
            # check if the current instruction defines a variable in the
            # dataflow workset

            while len(dataws) > 0:
                id = dataws.pop()
                pre, post = self.getSyscallByID(id)
                if post is None:
                    # skip
                    continue
    
                tmp = set()
                for v in post.defineVal("HANDLE"):
                    for p in preids:
                        if p <= post.getID():
                            # skip
                            continue
                        pre2, post2 = self.getSyscallByID(p)
                        if  (post2 is not None) and (v in post2.defineVal("HANDLE")):
                            # another syscall is redefining the same handle..
                            break
                        if v in pre2.useVal("HANDLE"):
                            slice.append(pre2.getID())
                            if not onestep:
                                dataws.append(pre2.getID())
            return slice


    def backwardSlice(self, startid, v):
        """
        backwardSlice(startid,v) calculates the backward slice for the
        instruction with ID 'startid', for variable v=(type_name,value).
        """
        dataws = [startid]
        slice = [startid]

        while len(dataws) > 0:
            # follow data dependencies
            # ------------------------
            # check if the current instruction defines a variable in the
            # dataflow workset

            while len(dataws) > 0:
                id = dataws.pop()
                pre, post = self.getSyscallByID(id)
                if pre is None:
                    # skip
                    continue
    
                tmp = set()
                for v in pre.useVal("HANDLE"):
                    rd = self.reachingDefinition(id, ("HANDLE", v))
                    if (rd is not None) and (rd not in slice):
                        slice.append(rd)
                        dataws.append(rd)
            return slice

    def normalize(self):
        """
        Performs normalization. At this level, we do those normalizations that
        needs both the pre & post syscall objects
        """
        import normalize

        for id in self:
            pre,post = self.getSyscallByID(id)

            normalize.normalize(pre, post)

    def analyse(self):
        """
        Performs analyses on this Syscall trace.
        """
        self.buildUseValMap()
        self.buildDefValMap()
