/**
 *  @file
 *  @copyright defined in LICENSE.txt
 */
#pragma once

#include <dp/static_variant.hpp>
#include <dp/time.hpp>
#include <dp/variant.hpp>
#include <dp/exception/exception.hpp>
#include <dp/network/url.hpp>

namespace dp {

class http_client {
   public:
      http_client();
      ~http_client();

      variant post_sync(const url& dest, const variant& payload, const time_point& deadline = time_point::maximum());

      template<typename T>
      variant post_sync(const url& dest, const T& payload, const time_point& deadline = time_point::maximum()) {
         variant payload_v;
         to_variant(payload, payload_v);
         return post_sync(dest, payload_v, deadline);
      }

      void add_cert(const std::string& cert_pem_string);
      void set_verify_peers(bool enabled);

private:
   std::unique_ptr<class http_client_impl> _my;
};

}