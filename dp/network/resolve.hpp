#pragma once
#include <dp/vector.hpp>
#include <dp/network/ip.hpp>

namespace dp
{
  std::vector<boost::asio::ip::udp::endpoint> resolve(boost::asio::io_service& io_service,
                                                      const std::string& host, uint16_t port);
}
