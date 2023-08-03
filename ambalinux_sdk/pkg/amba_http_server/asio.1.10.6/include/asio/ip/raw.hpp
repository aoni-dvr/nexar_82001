
//
// raw.hpp
// ~~~~~~~
//
// Copyright (c) 2003-2007 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IP_RAW_HPP
#define ASIO_IP_RAW_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "asio/detail/config.hpp"
#include "asio/basic_raw_socket.hpp"
#include "asio/ip/basic_endpoint.hpp"
#include "asio/ip/basic_resolver.hpp"
#include "asio/ip/basic_resolver_iterator.hpp"
#include "asio/ip/basic_resolver_query.hpp"
#include "asio/detail/socket_types.hpp"

#include "asio/detail/push_options.hpp"

namespace asio	{
namespace ip	{

/// Encapsulates the flags needed for RAW.
/**
 * The boost::asio::ip::raw class contains flags necessary for RAW sockets.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Safe.
 *
 * @par Concepts:
 * Protocol, InternetProtocol.
 */
class raw
{
public:
	/// The type of a RAW endpoint.
	typedef basic_endpoint<raw> endpoint;

	/// The type of a resolver query.
	typedef basic_resolver_query<raw> resolver_query;

	/// The type of a resolver iterator.
	typedef basic_resolver_iterator<raw> resolver_iterator;

	/// Construct to represent the IPv4 protocol.
	static raw v4(int protocol = IPPROTO_IP)
	{
		return raw(PF_INET, protocol);
	}

	/// Construct to represent the IPv6 protocol.
	static raw v6(int protocol = IPPROTO_IP)
	{
		return raw(PF_INET6, protocol);
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

	/// The RAW socket type.
	typedef basic_raw_socket<raw> socket;

	/// The RAW resolver type.
	typedef basic_resolver<raw> resolver;

	/// Socket option for enable/disabling the IP_HDRINCL.
	/**
	* Implements the IPPROTO_IP/IP_HDRINCL socket option.
	*
	* @par Examples
	* Setting the option:
	* @code
	* boost::asio::ip::raw::socket socket(io_service); 
	* ...
	* boost::asio::ip::raw::ip_hdrincl option(true);
	* socket.set_option(option);
	* @endcode
	*
	* @par
	* Getting the current option value:
	* @code
	* boost::asio::ip::raw::socket socket(io_service); 
	* ...
	* boost::asio::ip::raw::ip_hdrincl option;
	* socket.get_option(option);
	* bool is_set = option.value();
	* @endcode
	*
	* @par Concepts:
	* Socket_Option, Boolean_Socket_Option.
	*/
#if defined(GENERATING_DOCUMENTATION)
	typedef implementation_defined no_delay;
#else
	typedef asio::detail::socket_option::boolean<IPPROTO_IP, IP_HDRINCL> ip_hdrincl;
#endif

	/// Compare two protocols for equality.
	friend bool operator==(const raw& p1, const raw& p2)
	{
		return (p1.family_ == p2.family_ && p1.protocol_ == p2.protocol_);
	}

	/// Compare two protocols for inequality.
	friend bool operator!=(const raw& p1, const raw& p2)
	{
		return (p1.family_ != p2.family_ || p1.protocol_ != p2.protocol_);
	}

private:
	// Construct with a specific family.
	explicit raw(int family, int protocol)
	: family_(family)
	, protocol_(protocol)
	{
	}

	int family_;
	int protocol_;
};

}	//namespace ip
}	//namespace asio

#include "asio/detail/pop_options.hpp"

#endif	//ASIO_IP_RAW_HPP
