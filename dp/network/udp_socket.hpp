#pragma once
#include <dp/utility.hpp>
#include <dp/shared_ptr.hpp>
#include <memory>

#include <boost/asio.hpp>

namespace dp {
  namespace ip {
    class endpoint;
    class address;
  }

  /**
   *  The udp_socket class has reference semantics, all copies will
   *  refer to the same underlying socket.
   */
  class udp_socket {
    public:
      udp_socket();
      udp_socket( const udp_socket& s );
      ~udp_socket();

      void initialize(boost::asio::io_service &);
      void open();
      void send_to(const char* b, size_t l, boost::asio::ip::udp::endpoint &to);
      void send_to(const std::shared_ptr<const char>& b, size_t l, boost::asio::ip::udp::endpoint &to);
      void close();

      void set_reuse_address(bool);

      void connect(const boost::asio::ip::udp::endpoint& e);
      const boost::asio::ip::udp::endpoint local_endpoint() const;

    private:
      class                impl;
      dp::shared_ptr<impl> my;
  };

}
