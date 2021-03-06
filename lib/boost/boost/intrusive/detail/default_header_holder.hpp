/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2014-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_DEFAULT_HEADER_HOLDER_HPP
#define BOOST_INTRUSIVE_DETAIL_DEFAULT_HEADER_HOLDER_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/pointer_traits.hpp>
#include <boost/intrusive/detail/to_raw_pointer.hpp>

namespace boost {
namespace intrusive {
namespace detail {

// trivial header node holder
template < typename NodeTraits >
struct default_header_holder : public NodeTraits::node
{
   typedef NodeTraits node_traits;
   typedef typename node_traits::node node;
   typedef typename node_traits::node_ptr node_ptr;
   typedef typename node_traits::const_node_ptr const_node_ptr;

   default_header_holder() : node() {}

   const_node_ptr get_node() const
   { return pointer_traits< const_node_ptr >::pointer_to(*static_cast< const node* >(this)); }

   node_ptr get_node()
   { return pointer_traits< node_ptr >::pointer_to(*static_cast< node* >(this)); }

   // (unsafe) downcast used to implement container-from-iterator
   static default_header_holder* get_holder(const node_ptr &p)
   { return static_cast< default_header_holder* >(boost::intrusive::detail::to_raw_pointer(p)); }
};

// type function producing the header node holder
template < typename Value_Traits, typename HeaderHolder >
struct get_header_holder_type
{
   typedef HeaderHolder type;
};
template < typename Value_Traits >
struct get_header_holder_type< Value_Traits, void >
{
   typedef default_header_holder< typename Value_Traits::node_traits > type;
};

} //namespace detail
} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_DETAIL_DEFAULT_HEADER_HOLDER_HPP
