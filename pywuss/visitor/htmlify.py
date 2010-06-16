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

class VisitorHTML(visitor.Visitor):
    """Outputs the syscall trace in HTML format."""

    def accept_SyscallTrace(self, trace, *params, **extraparams):
        r = \
"""
<html>
  <head>
    <title>Syscall trace for %s</title>
    <script type="text/javascript">
      function toggleArg(id) {
        var tr = document.getElementById(id);
        if (tr == null) { return; }

        var bExpand = tr.style.display == '';
        tr.style.display = (bExpand ? 'none' : '');
      }
    </script>
    <style type="text/css">
      table.arg {
        font-size: 10px;
      }
    </style>
  </head>
  <body>
    <h1>Syscall trace for %s (PID: %d)</h1>
    <p>
     <em>Click on a system call to show/collapse its arguments.</em>
    </p>
      <table>
        <tr>
          <th>ID</th>
          <th>Name</th>
          <th>Direction</th>
          <th>Thread</th>
        </tr>
""" % (trace.getFilename(), trace.getFilename(), trace.getPID())

        for s in trace.getSyscalls():
            r += s.visit(self, *params, **extraparams)

        r += \
"""
      </table>
  </body>
</html>
"""
        return r

    def accept_Syscall(self, syscall, *params, **extraparams):
        style = ""

        if syscall.isPre():
            direction = "PRE"
        else:
            direction = "POST"
        argid = "%s-%s" % (syscall.getID(), direction)

        r = \
"""
<tr style="%s" onclick="javascript:toggleArg('%s');">
  <td>%s</td>
  <td>%s</td>
  <td>%s</td>
  <td>%s</td>
</tr>""" % (style, argid, syscall.getID(), syscall.getName(), direction, syscall.getThreadID())

        # build arguments row
        r += \
"""
<tr id="%s" style="display: none;">
  <td colspan="4">
    <table class="arg" border="1">""" % (argid)

        if len(syscall.getArgument()) > 0:
            for i in syscall.getArgument():
                r += "\n" + syscall.getArgument(i).visit(self, *params, **extraparams)
        else:
            # no arguments
            r += "<tr><td><em>no arguments</em></td></tr>"

        r += \
"""
    </table>
  </td>
</tr>
"""

        return r

    def accept_SyscallArg(self, arg, *params, **extraparams):
        r  = "<tr>"
        r += "<td>%d</td>"  % arg.getNum()
        r += "<td>%s (%s)</td>"  % (arg.getVal().getName(), arg.getVal().getType())
        r += "<td>%s</td>" % ((arg.isOptional() and "OPTIONAL") or "MANDATORY")
        r += "<td>%s</td>" % arg.getVal().visit(self, *params, **extraparams)
        r += "</tr>"

        return r

    def accept_BasicArgument(self, val, *params, **extraparams):
        return str(val.getValue())

    def accept_StructArgument(self, val, *params, **extraparams):
        r  = "<ul>"
        for n in val:
            f = val.getField(n)
            r += "<li>%s (%s)" % (f.getName(), f.getType())
            if f.isBasic():
                r += " = %s" % f.getValue()
            else:
                r += f.visit(self, *params, **extraparams)
            r += "</li>"
        r += "</ul>"
        
        return r
