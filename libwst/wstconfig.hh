// Copyright notice
// ================
//
// Copyright (C) 2010
//     Lorenzo Martignoni <martignlo@gmail.com>
//     Roberto Paleari <roberto.paleari@gmail.com>
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 3 of the License, or (at your option) any later
// version.  
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#ifndef _WST_CONFIG_H
#define _WST_CONFIG_H

#define WST_DEBUG_MODE
#define DISABLE_SERIALIZATION_IMPLEVEL

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

#define XML_NAME_TYPE_SEPARATOR "."

#ifdef WST_DEBUG_MODE
#ifndef WST_DEBUG_FILE
#define WST_DEBUG_FILE stderr
#endif
#ifndef WST_DEBUG
#define WST_DEBUG(fmt, ...) fprintf(WST_DEBUG_FILE, fmt, ##__VA_ARGS__)
#endif
#ifndef wst_debug
#define wst_debug(fmt, ...) fprintf(WST_DEBUG_FILE, fmt, ##__VA_ARGS__)
#endif
#else
#ifndef WST_DEBUG
#define WST_DEBUG(fmt, ...) /* nothing to do */
#endif
#ifndef wst_debug
#define wst_debug(fmt, ...) /* nothing to do */
#endif
#endif

// #define NEED_DETECTOR_HELPER

#endif	/* _WST_CONFIG_H */
