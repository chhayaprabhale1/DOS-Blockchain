#pragma once
#include <dp/io/json.hpp>
#include <dp/network/http/connection.hpp>
#include <dp/network/http/server.hpp>
#include <dp/reflect/variant.hpp>
#include <dp/rpc/api_connection.hpp>
#include <dp/rpc/state.hpp>

namespace dp { namespace rpc {

   class http_api_connection : public api_connection
   {
      public:
         http_api_connection();
         ~http_api_connection();

         virtual variant send_call(
            api_id_type api_id,
            string method_name,
            variants args = variants() ) override;
         virtual variant send_callback(
            uint64_t callback_id,
            variants args = variants() ) override;
         virtual void send_notice(
            uint64_t callback_id,
            variants args = variants() ) override;

         void on_request(
            const dp::http::request& req,
            const dp::http::server::response& resp );

         dp::rpc::state                   _rpc_state;
   };

} } // namespace dp::rpc
