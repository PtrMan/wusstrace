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

import psyco
psyco.full()

import os, sys, getopt
from lxml import etree
import dircache

from syscall import *
from debug import log, colorize, COLOR_YELLOW
from common import decode_base64, decode_unicode
from syscall_winxp import syscall_winxp_map
from config import SYSCALL_BLACKLIST

XMLSCHEMA_FILENAME = "../wusstrace.xsd"
XML_NAME_TYPE_SEPARATOR = "."
options = {}

def fixdocument(d):
    s = ""
    buffer = []
    for l in d.split("\n"):
        l = l.strip("\n\r")
        buffer.append(l)

        if l.strip() == "</Syscall>":
            s += "\n".join(buffer)
            del buffer[:]
    s += "</boost_serialization>"
    return s

def getText(node):
    val = node.text
    return val

def build_argument_value(root):
    assert XML_NAME_TYPE_SEPARATOR in root.tag and len(root.tag.split(XML_NAME_TYPE_SEPARATOR)) == 2, \
        "! invalid XML file: missing name/type separator in '%s'" % root.tag
    (typ, name) = root.tag.split(XML_NAME_TYPE_SEPARATOR)
            
    if len(root) == 0:
        # basic argument
        v = getText(root)
        return BasicArgument(name, typ, v)
    else:
        # compound argument
        s = StructArgument(name, typ)
        for c in root:
            if c.tag in ["Address", "Size"]: continue
            v = build_argument_value(c)
            s.addField(v)
        return s

def parse_syscall(node):
    """Builds a Syscall object starting from an XML Syscall element."""
    (id, ltime, htime, num, name, direction, tid) = (None, None, None, None, None, None, None)
    for c in node:
        if c.tag == "ID":
            id = getText(c)
            assert id.isdigit(), "! invalid XML file: SyscallID field must be a number"
            id = int(id)

            # check if this system call is blacklisted
            if (id in syscall_winxp_map) and (syscall_winxp_map[id] in SYSCALL_BLACKLIST):
                # skip it!
                return None
        elif c.tag =="Time":
            for subnode in c:
                (typ, name) = subnode.tag.split(XML_NAME_TYPE_SEPARATOR)
                if name == "LowPart":
                    ltime = getText(subnode)
                    assert ltime.isdigit(), "! invalid XML file: Time.LowPart field must be a number"
                    ltime = int(ltime)
                elif name == "HighPart":
                    htime = getText(subnode)
                    assert htime.isdigit(), "! invalid XML file: Time.HighPart field must be a number"
                    htime = int(htime)
        elif c.tag == "No":
            num = getText(c)
            assert num.isdigit(), "! invalid XML file: SyscallNo field must be a number"
            num = int(num)
        elif c.tag == "Dir":
            direction = getText(c)
            assert direction.isdigit() and int(direction) in [SyscallDir_IN, SyscallDir_OUT], \
                "! invalid XML file: SyscallDir field is invalid"
            direction = int(direction)
        elif c.tag == "ThreadID":
            tid = getText(c)
            assert tid.isdigit(), "! invalid XML file: ThreadID field must be a number"
            tid = int(tid)
    assert None not in [id,ltime,htime,num,direction,tid], "! XML document is incomplete"

    s = Syscall(idz = id, ltime = ltime, htime = htime, num = num, direction = direction, tid = tid)

    return s

def parse_argument(node):
    (num, name, direction, opt) = (None, None, None, None)

    for c in node:
        if c.tag == "No":
            num = getText(c)
            assert num.isdigit(), "! invalid XML file: ArgNumber field must be a number"
            num = int(num)
        elif c.tag == "Name":
            name = getText(c)
        elif c.tag == "Dir":
            direction = getText(c)
            assert direction.isdigit() and \
                int(direction) in [SyscallArgDir_IN, SyscallArgDir_OUT, SyscallArgDir_INOUT], \
                "! invalid XML file: ArgDirection field is invalid"
            direction = int(direction)
        elif c.tag == "Opt":
            opt = getText(c)
            assert opt.isdigit() and \
                int(opt) in [SyscallArgOpt_OPTIONAL, SyscallArgOpt_MANDATORY], \
                "! invalid XML file: ArgOptional field is invalid"
            if int(opt) == SyscallArgOpt_OPTIONAL:
                opt = True
            else:
                opt = False

    assert None not in [name,direction,opt], "! XML document is incomplete"

    # parse argument value
    val = build_argument_value(node[-1])
    return SyscallArg(num = num, name = name, direction = direction, optional = opt, value = val)

def build_trace(root, filename):
    trace = SyscallTrace(name = filename)
    for e in root:
        if e.tag == "PID":
            trace.setPID(int(getText(e)))
        elif e.tag == "Filename":
            trace.setFilename(getText(e))
        elif e.tag == "Environment":
            for v in e.iterfind("Variable"):
                name  = getText(v.find("Name"))
                value = getText(v.find("Value"))
                if name is None:
                    if "=" in value and len(value.split("=")) == 2:
                        name, value = value.split("=")
                        name  = name.strip()
                        value = value.strip()
                    else:
                        # skip this environment variable
                        continue
                trace.addEnvironmentVariable(name, value)
        else:
            assert e.tag == "Syscall"
            s = parse_syscall(e)
            s.setTrace(trace)

            if s is None:
                # this must be skipped
                continue

            trace.append(s)

            # build arguments
            for f in e.iterfind("Arg"):
                a = parse_argument(f)
                s.addArgument(a, a.getNum())

            tmp = e.find("Retval")
            if tmp is not None:
                s.addRetval(parse_argument(tmp))
    return trace

def parse_file(filename):
    global options

    log("\n#### FILE '%s' ####" % filename)

    log("   %-47s" % "* reading XML document:", False)
    f = open(filename, 'r')
    data = f.read()
    f.close()
    log("done!")        

    log("   %-47s" % "* building DOM tree:", False)
    try:
        try:
            root = etree.fromstring(data)
        except etree.XMLSyntaxError, e:
            # try to fix invalid document, if required
            if 'f' not in options:
                raise e

            data = fixdocument(data)
            root = etree.fromstring(data)
    except etree.XMLSyntaxError, e:
        print colorize("\n! %s is not a valid XML document" % filename)
        sys.exit(1)
    log("done!")

    log("   %-47s" % "* validating XML document:", False)
    try:
        schema.assertValid(root)
    except etree.DocumentInvalid, e:
        print colorize("\n! %s does not validate" % filename)
        print colorize(e, COLOR_YELLOW)
        sys.exit(1)
    log("done!")

    log("   %-47s" % "* building objects:", False)
    trace = build_trace(root, filename)
    log("done! (%d system calls)" % len(trace))

    log("   %-47s" % "* cleaning up:", False)
    del root 
    log("done| (name: '%s'; pid: %d)" % (trace.getFilename(), trace.getPID()))

    log("   %-47s" % "* normalizing system calls trace:", False)
    trace.normalize()
    log("done!")

    log("   %-47s" % "* analysing system calls trace:", False)
    trace.analyse()
    log("done!")

    return trace

def parse_files(filenames):
    global options

    data = {}
    for i in range(len(filenames)):
        filename = filenames[i]
        trace = parse_file(filename)

        data[filename] = trace
    log("")

    return data

###########################################################################

def usage():
    print >> sys.stderr, \
"""
== PyWUSS ==
 Copyright 2010, Lorenzo Martignoni and Roberto Paleari
 License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
 This  is  free  software:  you  are free to change and redistribute it.
 There is NO WARRANTY, to the extent permitted by law.

Syntax: %s [options] [filename|dirname]
 -f   try to fix corrupted XML documents
 -h   display this help screen and exit
 -m   analyze the supplied directory for multiple execution traces
 -v   be verbose (use twice for more verbosity)
""" % sys.argv[0]

if __name__ == "__main__":
    # parse command line options
    try:
        optlist, proc = getopt.getopt(sys.argv[1:], 'fhmv')
        options['v'] = 0
        for o, v in optlist:
            o = o.lstrip('-')
            if o == 'v':
                options['v'] += 1
            else:
                options[o] = v
        if len(proc) == 0:
            raise getopt.GetoptError('missing filename')
    except getopt.GetoptError:
        usage()
        sys.exit(1)

    # show help screen & exit
    if 'h' in options:
        usage()
        sys.exit(0)

    log("%-50s" % "* reading XMLSchema:", False)
    f = open(XMLSCHEMA_FILENAME, 'r')
    data = f.read()
    f.close()
    log("done!")

    log("%-50s" % "* building XMLSchema DOM tree:", False)
    try:
        schema = etree.XMLSchema(etree.fromstring(data))
    except etree.XMLSyntaxError, e:
        log("\n! %s is not a valid XML document" % XMLSCHEMA_FILENAME)
        log(str(e))
        sys.exit(1)
    log("done!")

    if os.path.isdir(proc[0]) and not proc[0].endswith("/"):
        proc[0] += "/"

    tracedirs = {}
    if 'm' not in options:
        # analyse a single dump file or a single execution trace directory
        filenames = []
        if os.path.isfile(proc[0]):
            filenames.append(proc[0])
            dirname = "."
        else:
            assert os.path.isdir(proc[0])
            dirname = proc[0]
            for f in dircache.listdir(dirname):
                f = dirname + f
                if os.path.isfile(f):
                    filenames.append(f)
        tracedirs[dirname] = filenames
    else:
        # analyse multiple trace directories, correlating together the single
        # execution traces
        assert os.path.isdir(proc[0]), "! '%s' must be a directory" % proc[0]

        for dirname in dircache.listdir(proc[0]):
            dirpath = proc[0] + dirname
            if not os.path.isdir(dirpath):
                continue
            if not dirpath.endswith("/"):
                dirpath += "/"

            filenames = []
            for f in dircache.listdir(dirpath):
                f = dirpath + f
                if not os.path.isfile(f):
                    continue
                filenames.append(f)
            tracedirs[dirname] = filenames

    dirs = tracedirs.keys()
    dirs.sort()

    executionsdata = []
    for dirname in dirs:
        filenames = tracedirs[dirname]

        s = "#### EXECUTION TRACE '%s' (%d files: %s) ####" % (dirname, len(filenames), [x[x.rfind("/")+1:] for x in filenames])
        log("#"*len(s))
        log(s)
        log("#"*len(s))

        filesdata = parse_files(filenames)
        executionsdata.append(filesdata)

    for e in executionsdata:
        for f, v in e.iteritems():
            print v
