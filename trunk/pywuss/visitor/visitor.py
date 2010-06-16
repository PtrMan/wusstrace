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

"""
Visitor
=======

Generic visitor. Each specialized visitor must inerith from this class as the
top-level accept method takes care of redirecting to the appropriate
specialized accept (actually dispacthing is performed guessing the name of
the class received in input, and if the appropriate accept is not found, the
same is applied with the superclasses names).

Multiple instantiations of the same visitor should result in a singleton.
"""

# record the instantiated classes
_instances = {}

class Singleton(object):
    def __new__(cls, *args, **kwargs):
        if not cls in _instances:
            ii = super(Singleton, cls).__new__(cls, *args, **kwargs)
            _instances[cls] = ii
        return _instances[cls]

class Visitable:
    """
    Generic visitable object.

    Each class that must be processed through a visitor may inherit this one.
    """
    def visit(self, visitor, *params, **extraparams):
        return visitor.accept(self, *params, **extraparams)

class Visitor(Singleton):
    """
    Visitor pattern.

    Every class that implements a visitor must be derived from this one such
    that every call to accept() is dispatched to the appropriate method.
    """
    
    def __init__(self):
        self.indent = 0

    def accept(self, other, *params, **extraparams):
        """Use reflection to dispatch the call to the right accept method"""

        # try to find the appropriate accept by traversing the class hierarchy
        method = None
        for m in other.__class__.__mro__:
	    if hasattr(self, "accept_" + m.__name__):
                method = self.__getattribute__("accept_" + m.__name__)
                break
        
        # if no valid accept is found, method = None
        if method is not None:
            return method(other, *params, **extraparams)
        else:
            raise NotImplementedError("method %s.accept_%s not yet implemented!" % \
                                          (self.__class__.__name__, other.__class__.__name__))

        return r
