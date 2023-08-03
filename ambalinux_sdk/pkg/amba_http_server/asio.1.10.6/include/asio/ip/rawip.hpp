
//
// rawip.hpp
// ~~~~~~~
//
// Copyright (c) 2003-2007 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_IP_RAWIP_HPP
#define BOOST_ASIO_IP_RAWIP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/basic_raw_socket.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/basic_resolver_iterator.hpp>
#include <boost/asio/ip/basic_resolver_query.hpp>
#include <boost/asio/detail/socket_types.hpp>

// NOTE: just for vnn4.0

namespace boost	{
namespace asio	{
namespace ip	{

/// Encapsulates the flags needed for RAWIP.
/**
 * The boost::asio::ip::rawip class contains flags necessary for RAWIP sockets.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Safe.
 *
 * @par Concepts:
 * Protocol, InternetProtocol.
 */
class rawip
{
public:
	/// The type of a RAWIP endpoint.
	typedef basic_endpoint<rawip> endpoint;

	/// The type of a resolver query.
	typedef basic_resolver_query<rawip> resolver_query;

	/// The type of a resolver iterator.
	typedef basic_resolver_iterator<rawip> resolver_iterator;

	/// Construct to represent the IPv4 protocol.
	static rawip v4()
	{
#ifdef WIN32
		return rawip(IPPROTO_IP, PF_INET);
#else		
		return rawip(IPPROTO_RAW, PF_INET);
#endif
	}

	/// Construct to represent the IPv6 protocol.
	static rawip v6()
	{
#ifdef WIN32
		return rawip(IPPROTO_IP, PF_INET6);
#else
		return rawip(IPPROTO_RAW, PF_INET6);
#endif
	}

	/// Obtain an identifier for the type of the protocol.
	int type() const
	{
		return SOCK_RAW;
	}

	/// Obtain an identifier for the protocol.
	int protocol() const
	{
		return protocol_;
	}

	/// Obtain an identifier for the protocol family.
	int family() const
	{
		return family_;
	}

	/// The RAWIP socket type.
	typedef basic_raw_socket<rawip> socket;

	/// The RAWIP resolver type.
	typedef basic_resolver<rawip> resolver;

#if defined(GENERATING_DOCUMENTATION)
	typedef implementation_defined no_delay;
#else
	typedef boost::asio::detail::socket_option::boolean<IPPROTO_IP, IP_HDRINCL> ip_hdrincl;
#endif

	/// Compare two protocols for equality.
	friend bool operator==(const rawip& p1, const rawip& p2)
	{
    return p1.protocol_ == p2.protocol_ && p1.family_ == p2.family_;
	}

	/// Compare two protocols for inequality.
	friend bool operator!=(const rawip& p1, const rawip& p2)
	{
    return p1.protocol_ != p2.protocol_ || p1.family_ != p2.family_;
	}

private:
	// Construct with a specific family.
	explicit rawip(int protocol,int family)
	: protocol_(protocol)
	, family_(family)
	{
	}
	
	int protocol_;
	int family_;
};

}	//namespace ip
}	//namespace asio
}	//namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif	//BOOST_ASIO_IP_RAWIP_HPP
