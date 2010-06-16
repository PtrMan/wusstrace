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

#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

#include "wstconfig.hh"

#if defined SERIALIZE_TEXT
// text serialization
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/binary_object.hpp>

#define MAKENVP(x,name) x
#define SERIALIZE_IARCHIVE boost::archive::text_iarchive
#define SERIALIZE_OARCHIVE boost::archive::text_oarchive

#elif defined SERIALIZE_XML
// XML serialization
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/binary_object.hpp>

#define MAKENVP(x,name) boost::serialization::make_nvp(name, x)
#define SERIALIZE_IARCHIVE boost::archive::xml_iarchive
#define SERIALIZE_OARCHIVE boost::archive::xml_oarchive

#elif defined SERIALIZE_BINARY
// binary serialization
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#define MAKENVP(x,name) x
#define SERIALIZE_IARCHIVE boost::archive::binary_iarchive
#define SERIALIZE_OARCHIVE boost::archive::binary_oarchive

#else
#error "Invalid archive type"
#endif

#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <map>

using namespace std;

#include "serialize_internal.hh"
#include "type_winxp.hh"
#include "syscall.hh"
#include "syscall_tracer.hh"

// ****************************************************************************

#define DESERIALIZE Archive::is_loading::value
#define SERIALIZE   Archive::is_saving::value

#include "blob.hh"
#define BLOB(x,y) boost::serialization::make_blob((void *) x, y)

// Save an object into an output archive
#define OBJ_SAVE(oa, x, y)			\
  oa << MAKENVP(x, y)

// Load an object from an input archive
#define OBJ_LOAD(oa, x, y)			\
  oa >> MAKENVP(x, y)

// Fetch an object from memory (invoke the appropriare dereferentiation
// function if needed)
extern obj_peek_t obj_peek;
#define OBJ_PEEK(addr, size, out, type)				\
  type out = NULL;							\
  if (obj_peek) {							\
    out = (type) malloc(size);						\
    assert(out);							\
    obj_peek((void *) (addr), (uint32_t) (size), (void *) (out));	\
  } else {								\
    out = (type) addr;							\
  }

// Destroy an object previously fetched from memory
#define OBJ_FREE(buf)				\
  if (obj_peek && buf) {			\
    free(buf);					\
  }

// This macro defines how to allocate the memory to store the deserialized
// object. 
extern obj_malloc_t obj_malloc;
extern obj_allocation_t OBJ_ALLOCATION;
#define OBJ_MALLOC(x,y, z)			\
  ((OBJ_ALLOCATION) ? ((void *) obj_malloc((void *) y, z)) : ((void *) x))

// Object serialization
#define OBJ_SERIALIZE(ar, x, y)			\
  {						\
    ar & MAKENVP(x,y);				\
  }

// Pointer serialization
#define PTR_SERIALIZE(ar, t, x, y)			       	\
  {								\
    uint32_t tmpobjaddr = (uint32_t) x;				\
    if (tmpobjaddr) {						\
      if (DESERIALIZE) {                                        \
	x = (t) OBJ_MALLOC(x, tmpobjaddr, sizeof(*(x)));	\
	OBJ_SERIALIZE(ar, *(x), y);				\
      } else {							\
	OBJ_PEEK(tmpobjaddr, sizeof(*(x)), k, t);		\
	OBJ_SERIALIZE(ar, *(k), y);				\
	OBJ_FREE(k);						\
      }								\
    } else {							\
      if (DESERIALIZE) {					\
	x = (t) tmpobjaddr;					\
      }	else {							\
	string tmpstring = string("");				\
	OBJ_SERIALIZE(ar, tmpstring, y);			\
      }								\
    }								\
  }

// Array serialization
#define ARRAY_SERIALIZE(ar, x, z, y)		\
  {						\
    uint32_t tmpsize = z * sizeof(*x);		\
    if (tmpsize) {				\
      OBJ_SERIALIZE(ar, tmpsize, "Size");	\
      if (DESERIALIZE) {			\
	OBJ_SERIALIZE(ar, BLOB(x, tmpsize), y);	\
      } else {					\
	OBJ_PEEK(x, y, k, uint8_t *);		\
	OBJ_SERIALIZE(ar, BLOB(k, tmpsize), y);	\
	OBJ_FREE(k);				\
      }						\
    }						\
  }					    

// Blob serialization
#define BLOB_SERIALIZE(ar, t, x, z, y)			\
  {							\
    uint32_t tmpobjaddr = (uint32_t) x;			\
    OBJ_SERIALIZE(ar, tmpobjaddr, "Address");           \
    OBJ_SERIALIZE(ar, z, "Size");                       \
    if (tmpobjaddr && z) {				\
      if (DESERIALIZE) {				\
	x = (t) OBJ_MALLOC(x, tmpobjaddr, z);		\
	OBJ_SERIALIZE(ar, BLOB(tmpobjaddr, z), y);	\
      }	else {						\
	OBJ_PEEK(tmpobjaddr, z, k, t);			\
	OBJ_SERIALIZE(ar, BLOB(k, z), y);		\
	OBJ_FREE(k);					\
      }							\
    } else {						\
      if (DESERIALIZE) {				\
	x = (t) tmpobjaddr;				\
      } else {						\
	string tmpstring = string("");			\
	OBJ_SERIALIZE(ar, tmpstring, y);		\
      }							\
    }							\
  }							

// Struct serialization
#define STRUCT_SERIALIZE(ar, t, x, y)		\
  {						\
    int size = sizeof(t);			\
    void *blob  = (void *) &x;			\
    BLOB_SERIALIZE(ar, void *, blob, size, y);	\
  }

#define MAKE_XML_NAME(typname, fieldname) (typname XML_NAME_TYPE_SEPARATOR fieldname)

// ****************************************************************************

BOOST_CLASS_IMPLEMENTATION(SyscallArg, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(SyscallArg, boost::serialization::track_never);
BOOST_CLASS_IMPLEMENTATION(Syscall, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(Syscall, boost::serialization::track_never);

template<class Archive>
void serialize(Archive & ar, SyscallArg & arg, const unsigned int version) {
  // wst_debug("   SERIALIZING ARG %s %x %d", syscallarg_name(at), ap, ap);
  string sn  = syscallarg_name(arg.at);
  string argvalname;
  size_t e = sn.find("_WST");
  if (e != string::npos) {
    sn.erase(e, 4);
  }
  argvalname = sn;
  argvalname.append(".");
  argvalname.append(arg.aname);

  OBJ_SERIALIZE(ar, arg.an, "No");
  // OBJ_SERIALIZE(ar, arg.at, "ArgType");
  OBJ_SERIALIZE(ar, sn, "Name");
  OBJ_SERIALIZE(ar, arg.ad, "Dir");
  OBJ_SERIALIZE(ar, arg.ao, "Opt");

  switch (arg.at) {
    // ***********************************************************************
    // Object

#define MagicSyscallArgTypeObj(t)			\
    case SyscallArgType_ ## t: {			\
      t tmpobj = (t) ((uint32_t) arg.ap);		\
      OBJ_SERIALIZE(ar, tmpobj, argvalname.c_str());	\
      break;						\
    }
#include "syscall_winxp_args.hh"
#undef MagicSyscallArgTypeObj

    // Pointer to object
#define MagicSyscallArgTypePtr(t)			\
    case SyscallArgType_ ## t: {			\
      t tmpobj = (t) ((uint32_t) arg.ap);		\
      PTR_SERIALIZE(ar, t, tmpobj, argvalname.c_str());	\
      break;						\
    }
#include "syscall_winxp_args.hh"			
#undef MagicSyscallArgTypePtr

    // Pointer to object (serialized opaquely)
#define MagicSyscallArgTypeBlob(t, s)					\
    case SyscallArgType_ ## t: {					\
      void *tmpobj = arg.ap;						\
      int tmpsize = sizeof(s);						\
      wst_debug("Blob addr: %ld; size: %ld", tmpobj, tmpsize);		\
      if (tmpsize) BLOB_SERIALIZE(ar, void *, arg.ap, tmpsize, argvalname.c_str()); \
      break;								\
    }
#include "syscall_winxp_args.hh"
#undef MagicSyscallArgTypeBlob

    // **********************************************************************
    // Manual argument serialization/deserialization. We deserialize void *
    // buffers at the same address (the buffer might contain pointers to
    // self).
  case SyscallArgType_PVOID_WST_SIZEBEFORE: 
  case SyscallArgType_PVOID_WST_SIZEAFTER: {
    BLOB_SERIALIZE(ar, void *, arg.ap, arg.as, argvalname.c_str());
    break;
  }

#if 0
  case SyscallArgType_PWSTR: {
    PWSTR tmp;
    ULONG len = 0;

    tmp = (PWSTR) arg.ap;
    if (tmp) {
      // Compute the size if we are serializing
      if (SERIALIZE) {
	len++;
	while (tmp != 0) {
	  tmp++;
	  len++;
	}
      }
      BLOB_SERIALIZE(ar, PWSTR, arg.ap, len, argvalname.c_str());
    }
    break;
  }
#endif

  case SyscallArgType_BOOLEAN_WST: {
    uint32_t b = ((uint32_t) arg.ap) & 0x1;
    OBJ_SERIALIZE(ar, b, argvalname.c_str());
    break;
  } 

  default: {
    // wst_debug("%s not yet supported!", syscallarg_name(arg.at));
    string msg = "unsupported";
    OBJ_SERIALIZE(ar, msg, argvalname.c_str());
    break;
  }
      
  } // end switch
}

template<class Archive>
void serialize(Archive & ar, Syscall & s, const unsigned int version) {
  SyscallArgDir d;

  OBJ_SERIALIZE(ar, s.id, "ID");
  OBJ_SERIALIZE(ar, s.timestamp, "Time");
  OBJ_SERIALIZE(ar, s.sysno, "No");
  string sn = syscall_name(s.sysno);
  OBJ_SERIALIZE(ar, sn, "Name");

  if (s.dir == SyscallDir_ENTER) {
    d = SyscallArgDir_IN;
  } else {
    d = SyscallArgDir_OUT;
  }

  OBJ_SERIALIZE(ar, d, "Dir");
  OBJ_SERIALIZE(ar, s.tid, "ThreadID");

  for (SyscallArgs::iterator it = s.args.begin(); it != s.args.end(); it++) {
    if ((*it)->getArgDir() & d) {
      OBJ_SERIALIZE(ar, **it, "Arg");
    }
  }

  if (s.dir == SyscallDir_EXIT) {
    OBJ_SERIALIZE(ar, *s.retval, "Retval");
  }
}

// ****************************************************************************
 
template<class Archive> 
inline void serialize(Archive &ar, LARGE_INTEGER_WST &li, const unsigned int fv) {
  OBJ_SERIALIZE(ar, li.LowPart, MAKE_XML_NAME("DWORD", "LowPart"));
  OBJ_SERIALIZE(ar, li.HighPart, MAKE_XML_NAME("LONG", "HighPart"));
}

template<class Archive> 
inline void serialize(Archive &ar, ULARGE_INTEGER_WST &ui, const unsigned int fv) {
  OBJ_SERIALIZE(ar, ui.LowPart, MAKE_XML_NAME("DWORD", "LowPart"));
  OBJ_SERIALIZE(ar, ui.HighPart, MAKE_XML_NAME("DWORD", "HighPart"));
}

template<class Archive> 
inline void serialize(Archive &ar, SID_IDENTIFIER_AUTHORITY_WST &i, const unsigned int fv) {
  // typedef struct {
  //    BYTE Value[6];
  // } SID_IDENTIFIER_AUTHORITY, *PSID_IDENTIFIER_AUTHORITY;

  ARRAY_SERIALIZE(ar, i.Value, 6, MAKE_XML_NAME("PBYTE", "Value"));
}

template<class Archive> 
inline void serialize(Archive &ar, SID_WST &s, const unsigned int fv) {
  // typedef struct _SID {
  //    UCHAR  Revision;
  //    UCHAR  SubAuthorityCount;
  //    SID_IDENTIFIER_AUTHORITY  IdentifierAuthority;
  //    ULONG  SubAuthority[ANYSIZE_ARRAY];
  // } SID, *PISID;

  OBJ_SERIALIZE(ar, s.Revision, MAKE_XML_NAME("UCHAR", "Revision"));
  OBJ_SERIALIZE(ar, s.SubAuthorityCount, MAKE_XML_NAME("UCHAR", "SubAuthorityCound"));
  OBJ_SERIALIZE(ar, s.IdentifierAuthority, MAKE_XML_NAME("SID_IDENTIFIER_AUTHORITY", "IdentifierAuthority"));
  ARRAY_SERIALIZE(ar, s.SubAuthority, ANYSIZE_ARRAY, MAKE_XML_NAME("ULONG", "SubAuthority"));  
}

template<class Archive> 
inline void serialize(Archive &ar, ACL_WST &a, const unsigned int fv) {
  // typedef struct _ACL {
  //    UCHAR  AclRevision;
  //    UCHAR  Sbz1;
  //    USHORT  AclSize;
  //    USHORT  AceCount;
  //    USHORT  Sbz2;
  // } ACL, *PACL;
  OBJ_SERIALIZE(ar, a.AclRevision, MAKE_XML_NAME("UCHAR", "AclRevision"));
  OBJ_SERIALIZE(ar, a.Sbz1, MAKE_XML_NAME("UCHAR", "Sbz1"));
  OBJ_SERIALIZE(ar, a.AclRevision, MAKE_XML_NAME("USHORT", "AclSize"));
  OBJ_SERIALIZE(ar, a.AceCount, MAKE_XML_NAME("USHORT", "AceCount"));
  OBJ_SERIALIZE(ar, a.Sbz2, MAKE_XML_NAME("USHORT", "Sbz2"));
}

// typedef struct _SECURITY_DESCRIPTOR {
//    UCHAR  Revision;
//    UCHAR  Sbz1;
//    SECURITY_DESCRIPTOR_CONTROL  Control;
//    PISID  Owner;
//    PISID  Group;
//    PACL  Sacl;
//    PACL  Dacl;
// } SECURITY_DESCRIPTOR, *PISECURITY_DESCRIPTOR;
template<class Archive> 
inline void serialize(Archive &ar, SECURITY_DESCRIPTOR_WST &sd, const unsigned int fv) {
  // STRUCT_SERIALIZE(ar, SECURITY_DESCRIPTOR, sd, "SECURITY_DESCRIPTOR");
  OBJ_SERIALIZE(ar, sd.Revision, MAKE_XML_NAME("UCHAR", "Revision"));
  OBJ_SERIALIZE(ar, sd.Sbz1, MAKE_XML_NAME("UCHAR", "Sbz1"));
  // typedef USHORT SECURITY_DESCRIPTOR_CONTROL, *PSECURITY_DESCRIPTOR_CONTROL;
  OBJ_SERIALIZE(ar, sd.Control, MAKE_XML_NAME("SECURITY_DESCRIPTOR_CONTROL", "Control"));
  PTR_SERIALIZE(ar, PISID, sd.Owner, MAKE_XML_NAME("PISID", "Owner"));
  PTR_SERIALIZE(ar, PISID, sd.Group, MAKE_XML_NAME("PISID", "Group"));
  PTR_SERIALIZE(ar, PACL, sd.Sacl, MAKE_XML_NAME("PACL", "Sacl"));
  PTR_SERIALIZE(ar, PACL, sd.Dacl, MAKE_XML_NAME("PACL", "Dacl"));
}

template<class Archive> 
inline void serialize(Archive &ar, SECURITY_QUALITY_OF_SERVICE_WST &qos, const unsigned int fv) {
  // typedef struct _SECURITY_QUALITY_OF_SERVICE {
  //   DWORD Length;
  //   SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
  //   SECURITY_CONTEXT_TRACKING_MODE ContextTrackingMode;
  //   BOOLEAN EffectiveOnly;
  // } SECURITY_QUALITY_OF_SERVICE, *PSECURITY_QUALITY_OF_SERVICE;
  OBJ_SERIALIZE(ar, qos.Length, MAKE_XML_NAME("DWORD", "Length"));
  // typedef enum _SECURITY_IMPERSONATION_LEVEL {
  //   SecurityAnonymous,
  //   SecurityIdentification,
  //   SecurityImpersonation,
  //   SecurityDelegation 
  // } SECURITY_IMPERSONATION_LEVEL, *PSECURITY_IMPERSONATION_LEVEL;
  OBJ_SERIALIZE(ar, qos.ImpersonationLevel, MAKE_XML_NAME("SECURITY_IMPERSONATION_LEVEL", "ImpersonationLevel"));
  // typedef char SECURITY_CONTEXT_TRACKING_MODE, *PSECURITY_CONTEXT_TRACKING_MODE;
  OBJ_SERIALIZE(ar, qos.ContextTrackingMode, MAKE_XML_NAME("SECURITY_CONTEXT_TRACKING_MODE", "ContextTrackingMode"));
  OBJ_SERIALIZE(ar, qos.EffectiveOnly, MAKE_XML_NAME("BOOLEAN", "EffectiveOnly"));
}


template<class Archive> 
inline void serialize(Archive &ar, IO_STATUS_BLOCK_WST &sc, const unsigned int fv) {
  OBJ_SERIALIZE(ar, sc.Status, MAKE_XML_NAME("NTSTATUS", "Status"));
  OBJ_SERIALIZE(ar, sc.Information, MAKE_XML_NAME("ULONG_PTR", "Information"));
}

template<class Archive> 
inline void serialize(Archive &ar, UNICODE_STRING_WST &us, const unsigned int fv) {
  // Seriazlized implicitly with us.Buffer
  OBJ_SERIALIZE(ar, us.Length, MAKE_XML_NAME("USHORT", "Length"));
  OBJ_SERIALIZE(ar, us.MaximumLength, MAKE_XML_NAME("USHORT", "MaximumLength"));
  BLOB_SERIALIZE(ar, PWSTR, us.Buffer, us.Length, MAKE_XML_NAME("PWSTR", "Buffer"));
}

template<class Archive> 
inline void serialize(Archive &ar, OBJECT_ATTRIBUTES_WST &oa, const unsigned int fv) {
  OBJ_SERIALIZE(ar, oa.Length, MAKE_XML_NAME("ULONG", "Length"));

#if 0
  if (!options.bResolveRelativePaths || !oa.RootDirectory) {
#endif
    OBJ_SERIALIZE(ar, oa.RootDirectory, MAKE_XML_NAME("HANDLE", "RootDirectory"));
    PTR_SERIALIZE(ar, PUNICODE_STRING_WST, oa.ObjectName, MAKE_XML_NAME("PUNICODE_STRING", "ObjectName"));
#if 0
  } else {
    // we must resolve the relative object name into an absolute one
    UNICODE_STRING tmp;

    if(resolveRelativeObject(&tmp, (HANDLE) oa.RootDirectory)) {
      // resolution OK
      HANDLE2 h;
      char *p;

      // merge together two unicode strings
      // we keep space for "\\\x0"
      tmp.Buffer = (PWSTR) realloc(tmp.Buffer, tmp.Length + oa.ObjectName->Length + 2);
      p = (char*) tmp.Buffer;

      p[tmp.Length]   = '\\';
      p[tmp.Length+1] = '\x0';
      memcpy(p + tmp.Length + 2, oa.ObjectName->Buffer, oa.ObjectName->Length);
      tmp.Length += oa.ObjectName->Length + 2;
      tmp.MaximumLength += oa.ObjectName->Length + 2;

      h = 0;
      OBJ_SERIALIZE(ar, h, MAKE_XML_NAME("HANDLE", "RootDirectory"));
      PTR_SERIALIZE(ar, PUNICODE_STRING, &tmp, MAKE_XML_NAME("PUNICODE_STRING", "ObjectName"));
      free(tmp.Buffer);
    } else {
      // we cannot resolve this name
      OBJ_SERIALIZE(ar, oa.RootDirectory, MAKE_XML_NAME("HANDLE", "RootDirectory"));
      PTR_SERIALIZE(ar, PUNICODE_STRING, oa.ObjectName, MAKE_XML_NAME("PUNICODE_STRING", "ObjectName"));
    }
  }
#endif

  OBJ_SERIALIZE(ar, oa.Attributes, MAKE_XML_NAME("ULONG", "Attributes"));


#if 0
  PTR_SERIALIZE(ar, PSECURITY_DESCRIPTOR2, oa.SecurityDescriptor, 
		MAKE_XML_NAME("PSECURITY_DESCRIPTOR", "SecurityDescriptor"));
#endif
  PTR_SERIALIZE(ar, PSECURITY_QUALITY_OF_SERVICE_WST, oa.SecurityQualityOfService, 
		MAKE_XML_NAME("PSECURITY_QUALITY_OF_SERVICE", "SecurityQualityOfService"));
}

template<class Archive> 
inline void serialize(Archive &ar, FILE_BASIC_INFORMATION_WST &fbi, 
		      const unsigned int fv) {
  OBJ_SERIALIZE(ar, fbi.CreationTime, MAKE_XML_NAME("LARGE_INTEGER", "CreationTime"));
  OBJ_SERIALIZE(ar, fbi.LastAccessTime, MAKE_XML_NAME("LARGE_INTEGER", "LastAccessTime"));
  OBJ_SERIALIZE(ar, fbi.LastWriteTime, MAKE_XML_NAME("LARGE_INTEGER", "LastWriteTime"));
  OBJ_SERIALIZE(ar, fbi.ChangeTime, MAKE_XML_NAME("LARGE_INTEGER", "ChangeTime"));
  OBJ_SERIALIZE(ar, fbi.FileAttributes, MAKE_XML_NAME("ULONG", "FileAttributes"));
}

template<class Archive> 
inline void serialize(Archive &ar, CLIENT_ID_WST &ci, const unsigned int fv) {
  // typedef struct _CLIENT_ID
  // {
  //    HANDLE UniqueProcess;
  //    HANDLE UniqueThread;
  // } CLIENT_ID, *PCLIENT_ID;
  
  OBJ_SERIALIZE(ar, ci.UniqueProcess, MAKE_XML_NAME("HANDLE", "UniqueProcess"));
  OBJ_SERIALIZE(ar, ci.UniqueThread, MAKE_XML_NAME("HANDLE", "UniqueThread"));
}

template<class Archive> 
inline void serialize(Archive &ar, PORT_MESSAGE_WST &pm, const unsigned int fv) {
  // typedef struct _PORT_MESSAGE_HEADER {
  //   USHORT DataSize;
  //   USHORT MessageSize;
  //   USHORT MessageType;
  //   USHORT VirtualRangesOffset;
  //   CLIENT_ID ClientId;
  //   ULONG MessageId;
  //   ULONG SectionSize;
  //   PUCHAR Data;
  // } PORT_MESSAGE_HEADER, *PPORT_MESSAGE_HEADER, PORT_MESSAGE, *PPORT_MESSAGE;
  
  OBJ_SERIALIZE(ar, pm.DataSize, MAKE_XML_NAME("USHORT", "DataSize"));
  OBJ_SERIALIZE(ar, pm.MessageSize, MAKE_XML_NAME("USHORT", "MessageSize"));
  OBJ_SERIALIZE(ar, pm.MessageType, MAKE_XML_NAME("USHORT", "MessageType"));
  OBJ_SERIALIZE(ar, pm.VirtualRangesOffset, MAKE_XML_NAME("USHORT", "VirtualRangesOffset"));
  OBJ_SERIALIZE(ar, pm.ClientId, MAKE_XML_NAME("CLIENT_ID", "ClientId"));
  OBJ_SERIALIZE(ar, pm.MessageId, MAKE_XML_NAME("ULONG", "MessageId"));
  OBJ_SERIALIZE(ar, pm.SectionSize, MAKE_XML_NAME("ULONG", "SectionSize"));
//   if (pm.DataSize > 0)
//     BLOB_SERIALIZE(ar, PUCHAR, pm.Data, pm.DataSize, MAKE_XML_NAME("PUCHAR", "Buffer"));
}

template<class Archive> 
inline void serialize(Archive &ar, CONTEXT_WST &ctx, const unsigned int fv) {
  STRUCT_SERIALIZE(ar, CONTEXT, ctx, MAKE_XML_NAME("CONTEXT", "CONTEXT"));
}

template<class Archive> 
inline void serialize(Archive &ar, TOKEN_INFORMATION_CLASS_WST &tic, const unsigned int fv) {
  STRUCT_SERIALIZE(ar, TOKEN_INFORMATION_CLASS, tic, MAKE_XML_NAME("TOKEN_INFORMATION_CLASS", "TOKEN_INFORMATION_CLASS"));
}

template<class Archive> 
inline void serialize(Archive &ar, struct timeval &tv, const unsigned int fv) {
  OBJ_SERIALIZE(ar, tv.tv_usec, MAKE_XML_NAME("DWORD", "LowPart"));
  OBJ_SERIALIZE(ar, tv.tv_sec,  MAKE_XML_NAME("DWORD", "HighPart"));
}

#ifdef DISABLE_SERIALIZATION_IMPLEVEL
// disable version tracking
BOOST_CLASS_IMPLEMENTATION(struct timeval, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(LARGE_INTEGER_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(ULARGE_INTEGER_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(SID_IDENTIFIER_AUTHORITY_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(SID_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(ACL_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(SECURITY_DESCRIPTOR_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(SECURITY_QUALITY_OF_SERVICE_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(IO_STATUS_BLOCK_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(UNICODE_STRING_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(OBJECT_ATTRIBUTES_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(FILE_BASIC_INFORMATION_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(CLIENT_ID_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(PORT_MESSAGE_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(CONTEXT_WST, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(TOKEN_INFORMATION_CLASS_WST, boost::serialization::object_serializable);
#endif

#endif
