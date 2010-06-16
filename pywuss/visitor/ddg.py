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

import visitor
from   syscall import *

class VisitorDDG(visitor.Visitor):
    """Outputs the data dependence graph in dot format."""

    def accept_SyscallTrace(self, trace, *params, **extraparams):
        # compute successors of each Syscall object
        usedefs = {}
        for id in trace:
            pre, post = trace.getSyscallByID(id)
            if pre is None:
                # skip
                continue
            tmp = set()
            for v in pre.useVal("HANDLE"):
                rd = trace.reachingDefinition(id, ("HANDLE", v))
                if rd is not None:
                    tmp.add(rd)
            if len(tmp) > 0:
                usedefs[id] = tmp
        definers = set()
        for v in usedefs.values():
            definers.update(v)
    
	r  = "digraph G {\n"
	r += "node [shape=\"box\" fontname=\"courier\" fontsize=\"10\"];\n"
	r += "edge [arrowsize=\"0.5\"];\n"
	r += "subgraph \"%s\" {\n" % (trace.getName())

        for id in trace:
            pre, post = trace.getSyscallByID(id)
            if pre is None:
                # mmmmh
                continue
	    if extraparams.get("prune") and (id not in usedefs) and (id not in definers):
                # skip "single", not referenced nodes
                continue
            r += pre.visit(self, *params, **extraparams) + "\n"
	r += "\n"

        for id in trace:
            if id not in usedefs:
                continue

            for t in usedefs[id]:
                r += "\"%d\" -> \"%d\";\n" % (id, t)

        r += "}\n"
        r += "}\n"

        return r

    def accept_Syscall(self, syscall, *params, **extraparams):
	r = "\"%d\" [label=\"%s\"];" % (syscall.getID(), syscall.getName())
        return r
