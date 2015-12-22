# Introduction #

WUSSTrace is a user-space syscall tracer for Microsoft Windows developed for
fine grained syscall tracing: it supports the majority of Windows system calls
(except GUI system calls), the majority of argument types, and dumps faithfully
all the supported types. WUSSTrace produces easy-to-parse XML traces leveraging
the Boost serialization library. Tracing is performed by injecting a shared
library in the address space of the traced process and by hooking the stubs
`KiFastSystemCall` and `KiIntSystemCall` in ntdll.dll.

# Installation #

In order to compile WUSSTrace you need the MinGW compiler and the Boost
serialization library. You can download a version of GCC that already contains the Boost libraries from [here](http://nuwen.net/mingw.html). Once your development environment is ready simply run `make`.

# How to use WUSSTrace #

To trace the execution of a program you just have to run WUSSTrace and specify
on the command line the name of the program to trace. By default WUSSTrace does
not dump on disk the system calls intercepted. To do that you have to add the
options `-d PATH/PREFIX` to the command line to specify where to store the
traces. To find you other valid command line options run WUSSTrace with no
arguments.

The system calls traces produced are XML documents. WUSSTrace comes with a
Python interface, called PyWUSS, that facilitates the analysis of XML traces.

# Limitations #

At the moment WUSSTrace only intercepts system calls invoked using
`KiIntSystemCall`. Support for hooking `KiFastSystemCall` has not been implemented
yet only because of time constraint. Consequently, if your machine supports
`sysenter` WUSSTrace will not work. To workaround this limitation you can
force applications to use `KiIntSystemCall` instead of `KiFastSystemCall`. Go to `HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\kernel`, create an entry named `FastSystemCallDisable` with a DWORD value of 1, and reboot.

WUSSTrace has been tested only on Microsoft Windows XP SP2.

# Copyright #

WUSSTrace is copyrighted (2010) by Lorenzo Martignoni and Roberto Paleari.

It is licensed under GPLv3+ (GNU GPL version 3 or later
<http://gnu.org/licenses/gpl.html>). For further information about the license
please read the file COPYING included in the distribution.

# Contact #

Bug reports, patch submissions, and any other useful contribution are very
welcome; just drop an email to:

  * Lorenzo Martignoni <martignlo@gmail.com>

  * Roberto Paleari <roberto.paleari@gmail.com>