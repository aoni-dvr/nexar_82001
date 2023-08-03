#pragma once

#ifdef _WIN32
#define ENABLE_LOG
#endif

#ifdef _WIN32
#pragma warning (disable:4503)
#pragma warning (disable:4996)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WIN32_WINNT 0x0601
#endif

#ifdef _WIN32
#include <SDKDDKVer.h>
#endif
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string>
#include <memory>
#include <algorithm>
#include <thread>
#include <condition_variable>
#include <queue>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <array>
#include <atomic>
#include <numeric>
#include <chrono>
#include <limits>
#include <time.h>
#include "asio.1.10.6/include/asio.hpp"
#ifndef _WIN32
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <sys/wait.h>
#else
#include <io.h>
#include <WinSock2.h>
#include <ws2def.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <Ws2tcpip.h>
#include <shlobj.h>
#include <raserror.h>
#include <winioctl.h>
#include <crtdbg.h>
#include <direct.h>
#endif

#ifdef _WIN32
#define strtok_r strtok_s
#define snprintf _snprintf
#define HAVE_TIMEGM
#define timegm _mkgmtime
#endif

#if (defined(_MSC_VER) && _MSC_VER < 1900)
#define constexpr
#define noexcept
#endif

typedef asio::ip::tcp::endpoint tcp_endpoint;
typedef asio::ip::udp::endpoint udp_endpoint;
typedef asio::ip::tcp::socket tcp_socket;
typedef asio::ip::udp::socket udp_socket;
typedef std::shared_ptr<tcp_socket> tcp_socket_ptr;
typedef std::shared_ptr<udp_socket> udp_socket_ptr;
typedef asio::ip::tcp::resolver tcp_resolver;
typedef asio::ip::udp::resolver udp_resolver;
typedef std::shared_ptr<udp_endpoint> udp_endpoint_ptr;

using std::chrono::steady_clock;
using asio::ip::address_v4;
typedef std::unique_ptr<FILE, decltype(&fclose)> FileUniquePtr;

#include "lambda_evil_wrap.h"
#include "log.h"
#include "config.h"
#include "misc.h"
#include "http/http_parser.h"
#include "http/cpp_http_parser.h"
