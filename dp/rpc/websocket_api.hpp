#pragma once
#include <dp/rpc/api_connection.hpp>
#include <dp/rpc/state.hpp>
#include <dp/network/http/websocket.hpp>
#include <dp/io/json.hpp>
#include <dp/reflect/variant.hpp>

namespace dp { namespace rpc {

   class websocket_api_connection : public api_connection
   {
      public:
         websocket_api_connection( dp::http::websocket_connection& c );
         ~websocket_api_connection();

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

      protected:
         std::string on_message(
            const std::string& message,
            bool send_message = true );

         dp::http::websocket_connection&  _connection;
         dp::rpc::state                   _rpc_state;
   };

} } // namespace dp::rpc
