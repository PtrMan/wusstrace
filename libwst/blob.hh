#ifndef __BLOB_H__
#define __BLOB_H__

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// nvp.hpp: interface for serialization system.

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//  See http://www.boost.org for updates, documentation, and revision history.

#include <cassert>

#include <cstddef> // std::size_t
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
  using ::size_t; 
} // namespace std
#endif

#include <boost/preprocessor/stringize.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/level.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/wrapper.hpp>

#include "wstconfig.hh"

namespace boost {
  namespace serialization {

    struct blob {
      const void * const m_t;
      const std::size_t m_size;

      template<class Archive>
      void save(Archive & ar, const unsigned int /* file_version */) const {
#ifndef SERIALIZE_BINARY
	if (m_size % 6 != 0) {
	  void *m_t2;
	  m_t2 = malloc(m_size + 6);
	  //wst_debug("m_size: %ld; m_t2: %ld", m_size, m_t2);
	  if (m_t2) {
		assert(m_t2);
		memset(m_t2, 0, m_size + 6);
		memcpy(m_t2, m_t, m_size);
		ar.save_binary(m_t2, m_size);
	  }
	  free(m_t2);
	} else {
	  ar.save_binary(m_t, m_size);
	}
#else
	ar.save_binary(m_t, m_size);
#endif
      }

      template<class Archive>
      void load(Archive & ar, const unsigned int /* file_version */) const {
#ifndef SERIALIZE_BINARY
	if (m_size % 6 != 0) {
	  void *m_t2;
	  m_t2 = malloc(m_size + 6);
	  //if (m_t2) {
		assert(m_t2);
		ar.load_binary(const_cast<void *>(m_t2), m_size);
		memcpy(m_t, m_t2, m_size);
	  //}
	  free(m_t2);
	} else {
	  ar.load_binary(const_cast<void *>(m_t), m_size);
	}
#else
	ar.load_binary(const_cast<void *>(m_t), m_size);
#endif
      }

      BOOST_SERIALIZATION_SPLIT_MEMBER()

      blob(const void * const t, std::size_t size) :
      m_t(t),
        m_size(size)
      {}
      blob(const binary_object & rhs) :
        m_t(rhs.m_t),
        m_size(rhs.m_size)
      {}
    };

    // just a little helper to support the convention that all serialization
    // wrappers follow the naming convention make_xxxxx
    inline 
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    const
#endif
    blob 
    make_blob(const void * t, std::size_t size){
      return blob(t, size);
    }

    // this is a wrapper
    template <>
    struct is_wrapper<blob>
      : public mpl::true_
    {};



  } // namespace serialization
} // boost

// don't need versioning info for this type
BOOST_CLASS_IMPLEMENTATION(
			   blob, 
			   boost::serialization::object_serializable
			   )

// don't track binary objects - usually they will be created on the stack
// and tracking algorithm (which uses the object address) might get
// confused.  note that these address will likely be members of some
// other structure which itself is tracked, so as a practical matter
// suppressing tracking shouldn't cause any redundancy.

BOOST_CLASS_TRACKING(blob, boost::serialization::track_never) 

#endif 
