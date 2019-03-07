/**
 *  @file
 *  @copyright defined in dos/LICENSE.txt
 */
#include <dosio/txn_test_gen_plugin/txn_test_gen_plugin.hpp>
#include <dosio/chain_plugin/chain_plugin.hpp>
#include <dosio/chain/wast_to_wasm.hpp>

#include <dp/variant.hpp>
#include <dp/io/json.hpp>
#include <dp/exception/exception.hpp>
#include <dp/reflect/variant.hpp>
#include <dp/io/json.hpp>

#include <boost/asio/high_resolution_timer.hpp>
#include <boost/algorithm/clamp.hpp>

#include <Inline/BasicTypes.h>
#include <IR/Module.h>
#include <IR/Validate.h>
#include <WAST/WAST.h>
#include <WASM/WASM.h>
#include <Runtime/Runtime.h>

#include <dosio.token/dosio.token.wast.hpp>
#include <dosio.token/dosio.token.abi.hpp>

namespace dosio { namespace detail {
  struct txn_test_gen_empty {};
}}

DP_REFLECT(dosio::detail::txn_test_gen_empty, );

namespace dosio {

static appbase::abstract_plugin& _txn_test_gen_plugin = app().register_plugin<txn_test_gen_plugin>();

using namespace dosio::chain;

#define CALL(api_name, api_handle, call_name, INVOKE, http_response_code) \
{std::string("/v1/" #api_name "/" #call_name), \
   [this](string, string body, url_response_callback cb) mutable { \
          try { \
             if (body.empty()) body = "{}"; \
             INVOKE \
             cb(http_response_code, dp::json::to_string(result)); \
          } catch (...) { \
             http_plugin::handle_exception(#api_name, #call_name, body, cb); \
          } \
       }}

#define INVOKE_V_R_R_R(api_handle, call_name, in_param0, in_param1, in_param2) \
     const auto& vs = dp::json::json::from_string(body).as<dp::variants>(); \
     api_handle->call_name(vs.at(0).as<in_param0>(), vs.at(1).as<in_param1>(), vs.at(2).as<in_param2>()); \
     dosio::detail::txn_test_gen_empty result;

#define INVOKE_V_R_R(api_handle, call_name, in_param0, in_param1) \
     const auto& vs = dp::json::json::from_string(body).as<dp::variants>(); \
     api_handle->call_name(vs.at(0).as<in_param0>(), vs.at(1).as<in_param1>()); \
     dosio::detail::txn_test_gen_empty result;

#define INVOKE_V_V(api_handle, call_name) \
     api_handle->call_name(); \
     dosio::detail::txn_test_gen_empty result;

#define CALL_ASYNC(api_name, api_handle, call_name, INVOKE, http_response_code) \
{std::string("/v1/" #api_name "/" #call_name), \
   [this](string, string body, url_response_callback cb) mutable { \
      if (body.empty()) body = "{}"; \
      auto result_handler = [cb, body](const dp::exception_ptr& e) {\
         if (e) {\
            try {\
               e->dynamic_rethrow_exception();\
            } catch (...) {\
               http_plugin::handle_exception(#api_name, #call_name, body, cb);\
            }\
         } else {\
            cb(http_response_code, dp::json::to_string(dosio::detail::txn_test_gen_empty())); \
         }\
      };\
      INVOKE \
   }\
}

#define INVOKE_ASYNC_R_R(api_handle, call_name, in_param0, in_param1) \
   const auto& vs = dp::json::json::from_string(body).as<dp::variants>(); \
   api_handle->call_name(vs.at(0).as<in_param0>(), vs.at(1).as<in_param1>(), result_handler);

struct txn_test_gen_plugin_impl {
   static void push_next_transaction(const std::shared_ptr<std::vector<signed_transaction>>& trxs, size_t index, const std::function<void(const dp::exception_ptr&)>& next ) {
      chain_plugin& cp = app().get_plugin<chain_plugin>();
      cp.accept_transaction( packed_transaction(trxs->at(index)), [=](const dp::static_variant<dp::exception_ptr, transaction_trace_ptr>& result){
         if (result.contains<dp::exception_ptr>()) {
            next(result.get<dp::exception_ptr>());
         } else {
            if (index + 1 < trxs->size()) {
               push_next_transaction(trxs, index + 1, next);
            } else {
               next(nullptr);
            }
         }
      });
   }

   void push_transactions( std::vector<signed_transaction>&& trxs, const std::function<void(dp::exception_ptr)>& next ) {
      auto trxs_copy = std::make_shared<std::decay_t<decltype(trxs)>>(std::move(trxs));
      push_next_transaction(trxs_copy, 0, next);
   }

   void create_test_accounts(const std::string& init_name, const std::string& init_priv_key, const std::function<void(const dp::exception_ptr&)>& next) {
      std::vector<signed_transaction> trxs;
      trxs.reserve(2);

      try {
         name newaccountA("txn.test.a");
         name newaccountB("txn.test.b");
         name newaccountC("txn.test.t");
         name creator(init_name);

         abi_def currency_abi_def = dp::json::from_string(dosio_token_abi).as<abi_def>();

         controller& cc = app().get_plugin<chain_plugin>().chain();
         auto chainid = app().get_plugin<chain_plugin>().get_chain_id();
         auto abi_serializer_max_time = app().get_plugin<chain_plugin>().get_abi_serializer_max_time();

         abi_serializer dosio_token_serializer{dp::json::from_string(dosio_token_abi).as<abi_def>(), abi_serializer_max_time};

         dp::crypto::private_key txn_test_receiver_A_priv_key = dp::crypto::private_key::regenerate(dp::sha256(std::string(64, 'a')));
         dp::crypto::private_key txn_test_receiver_B_priv_key = dp::crypto::private_key::regenerate(dp::sha256(std::string(64, 'b')));
         dp::crypto::private_key txn_test_receiver_C_priv_key = dp::crypto::private_key::regenerate(dp::sha256(std::string(64, 'c')));
         dp::crypto::public_key  txn_text_receiver_A_pub_key = txn_test_receiver_A_priv_key.get_public_key();
         dp::crypto::public_key  txn_text_receiver_B_pub_key = txn_test_receiver_B_priv_key.get_public_key();
         dp::crypto::public_key  txn_text_receiver_C_pub_key = txn_test_receiver_C_priv_key.get_public_key();
         dp::crypto::private_key creator_priv_key = dp::crypto::private_key(init_priv_key);

         //create some test accounts
         {
            signed_transaction trx;

            //create "A" account
            {
            auto owner_auth   = dosio::chain::authority{1, {{txn_text_receiver_A_pub_key, 1}}, {}};
            auto active_auth  = dosio::chain::authority{1, {{txn_text_receiver_A_pub_key, 1}}, {}};

            trx.actions.emplace_back(vector<chain::permission_level>{{creator,"active"}}, newaccount{creator, newaccountA, owner_auth, active_auth});
            }
            //create "B" account
            {
            auto owner_auth   = dosio::chain::authority{1, {{txn_text_receiver_B_pub_key, 1}}, {}};
            auto active_auth  = dosio::chain::authority{1, {{txn_text_receiver_B_pub_key, 1}}, {}};

            trx.actions.emplace_back(vector<chain::permission_level>{{creator,"active"}}, newaccount{creator, newaccountB, owner_auth, active_auth});
            }
            //create "txn.test.t" account
            {
            auto owner_auth   = dosio::chain::authority{1, {{txn_text_receiver_C_pub_key, 1}}, {}};
            auto active_auth  = dosio::chain::authority{1, {{txn_text_receiver_C_pub_key, 1}}, {}};

            trx.actions.emplace_back(vector<chain::permission_level>{{creator,"active"}}, newaccount{creator, newaccountC, owner_auth, active_auth});
            }

            trx.expiration = cc.head_block_time() + dp::seconds(30);
            trx.set_reference_block(cc.head_block_id());
            trx.sign(creator_priv_key, chainid);
            trxs.emplace_back(std::move(trx));
         }

         //set txn.test.t contract to dosio.token & initialize it
         {
            signed_transaction trx;

            vector<uint8_t> wasm = wast_to_wasm(std::string(dosio_token_wast));

            setcode handler;
            handler.account = newaccountC;
            handler.code.assign(wasm.begin(), wasm.end());

            trx.actions.emplace_back( vector<chain::permission_level>{{newaccountC,"active"}}, handler);

            {
               setabi handler;
               handler.account = newaccountC;
               handler.abi = dp::raw::pack(json::from_string(dosio_token_abi).as<abi_def>());
               trx.actions.emplace_back( vector<chain::permission_level>{{newaccountC,"active"}}, handler);
            }

            {
               action act;
               act.account = N(txn.test.t);
               act.name = N(create);
               act.authorization = vector<permission_level>{{newaccountC,config::active_name}};
               act.data = dosio_token_serializer.variant_to_binary("create", dp::json::from_string("{\"issuer\":\"txn.test.t\",\"maximum_supply\":\"1000000000.0000 CUR\"}}"), abi_serializer_max_time);
               trx.actions.push_back(act);
            }
            {
               action act;
               act.account = N(txn.test.t);
               act.name = N(issue);
               act.authorization = vector<permission_level>{{newaccountC,config::active_name}};
               act.data = dosio_token_serializer.variant_to_binary("issue", dp::json::from_string("{\"to\":\"txn.test.t\",\"quantity\":\"600.0000 CUR\",\"memo\":\"\"}"), abi_serializer_max_time);
               trx.actions.push_back(act);
            }
            {
               action act;
               act.account = N(txn.test.t);
               act.name = N(transfer);
               act.authorization = vector<permission_level>{{newaccountC,config::active_name}};
               act.data = dosio_token_serializer.variant_to_binary("transfer", dp::json::from_string("{\"from\":\"txn.test.t\",\"to\":\"txn.test.a\",\"quantity\":\"200.0000 CUR\",\"memo\":\"\"}"), abi_serializer_max_time);
               trx.actions.push_back(act);
            }
            {
               action act;
               act.account = N(txn.test.t);
               act.name = N(transfer);
               act.authorization = vector<permission_level>{{newaccountC,config::active_name}};
               act.data = dosio_token_serializer.variant_to_binary("transfer", dp::json::from_string("{\"from\":\"txn.test.t\",\"to\":\"txn.test.b\",\"quantity\":\"200.0000 CUR\",\"memo\":\"\"}"), abi_serializer_max_time);
               trx.actions.push_back(act);
            }

            trx.expiration = cc.head_block_time() + dp::seconds(30);
            trx.set_reference_block(cc.head_block_id());
            trx.max_net_usage_words = 5000;
            trx.sign(txn_test_receiver_C_priv_key, chainid);
            trxs.emplace_back(std::move(trx));
         }
      } catch (const dp::exception& e) {
         next(e.dynamic_copy_exception());
         return;
      }

      push_transactions(std::move(trxs), next);
   }

   void start_generation(const std::string& salt, const uint64_t& period, const uint64_t& batch_size) {
      if(running)
         throw dp::exception(dp::invalid_operation_exception_code);
      if(period < 1 || period > 2500)
         throw dp::exception(dp::invalid_operation_exception_code);
      if(batch_size < 1 || batch_size > 250)
         throw dp::exception(dp::invalid_operation_exception_code);
      if(batch_size & 1)
         throw dp::exception(dp::invalid_operation_exception_code);

      running = true;

      controller& cc = app().get_plugin<chain_plugin>().chain();
      auto abi_serializer_max_time = app().get_plugin<chain_plugin>().get_abi_serializer_max_time();
      abi_serializer dosio_token_serializer{dp::json::from_string(dosio_token_abi).as<abi_def>(), abi_serializer_max_time};
      //create the actions here
      act_a_to_b.account = N(txn.test.t);
      act_a_to_b.name = N(transfer);
      act_a_to_b.authorization = vector<permission_level>{{name("txn.test.a"),config::active_name}};
      act_a_to_b.data = dosio_token_serializer.variant_to_binary("transfer", 
                                                                  dp::json::from_string(dp::format_string("{\"from\":\"txn.test.a\",\"to\":\"txn.test.b\",\"quantity\":\"1.0000 CUR\",\"memo\":\"${l}\"}", 
                                                                  dp::mutable_variant_object()("l", salt))),
                                                                  abi_serializer_max_time);

      act_b_to_a.account = N(txn.test.t);
      act_b_to_a.name = N(transfer);
      act_b_to_a.authorization = vector<permission_level>{{name("txn.test.b"),config::active_name}};
      act_b_to_a.data = dosio_token_serializer.variant_to_binary("transfer", 
                                                                  dp::json::from_string(dp::format_string("{\"from\":\"txn.test.b\",\"to\":\"txn.test.a\",\"quantity\":\"1.0000 CUR\",\"memo\":\"${l}\"}", 
                                                                  dp::mutable_variant_object()("l", salt))),
                                                                  abi_serializer_max_time);

      timer_timeout = period;
      batch = batch_size/2;

      ilog("Started transaction test plugin; performing ${p} transactions every ${m}ms", ("p", batch_size)("m", period));

      arm_timer(boost::asio::high_resolution_timer::clock_type::now());
   }

   void arm_timer(boost::asio::high_resolution_timer::time_point s) {
      timer.expires_at(s + std::chrono::milliseconds(timer_timeout));
      timer.async_wait([this](const boost::system::error_code& ec) {
         if(!running || ec)
            return;

         send_transaction([this](const dp::exception_ptr& e){
            if (e) {
               elog("pushing transaction failed: ${e}", ("e", e->to_detail_string()));
               stop_generation();
            } else {
               arm_timer(timer.expires_at());
            }
         });
      });
   }

   void send_transaction(std::function<void(const dp::exception_ptr&)> next) {
      std::vector<signed_transaction> trxs;
      trxs.reserve(2*batch);

      try {
         controller& cc = app().get_plugin<chain_plugin>().chain();
         auto chainid = app().get_plugin<chain_plugin>().get_chain_id();
         auto abi_serializer_max_time = app().get_plugin<chain_plugin>().get_abi_serializer_max_time();

         dp::crypto::private_key a_priv_key = dp::crypto::private_key::regenerate(dp::sha256(std::string(64, 'a')));
         dp::crypto::private_key b_priv_key = dp::crypto::private_key::regenerate(dp::sha256(std::string(64, 'b')));

         static uint64_t nonce = static_cast<uint64_t>(dp::time_point::now().sec_since_epoch()) << 32;
         abi_serializer dosio_serializer(cc.db().find<account_object, by_name>(config::system_account_name)->get_abi(), abi_serializer_max_time);

         uint32_t reference_block_num = cc.last_irreversible_block_num();
         if (txn_reference_block_lag >= 0) {
            reference_block_num = cc.head_block_num();
            if (reference_block_num <= (uint32_t)txn_reference_block_lag) {
               reference_block_num = 0;
            } else {
               reference_block_num -= (uint32_t)txn_reference_block_lag;
            }
         }

         block_id_type reference_block_id = cc.get_block_id_for_num(reference_block_num);

         for(unsigned int i = 0; i < batch; ++i) {
         {
         signed_transaction trx;
         trx.actions.push_back(act_a_to_b);
         trx.context_free_actions.emplace_back(action({}, config::null_account_name, "nonce", dp::raw::pack(nonce++)));
         trx.set_reference_block(reference_block_id);
         trx.expiration = cc.head_block_time() + dp::seconds(30);
         trx.max_net_usage_words = 100;
         trx.sign(a_priv_key, chainid);
         trxs.emplace_back(std::move(trx));
         }

         {
         signed_transaction trx;
         trx.actions.push_back(act_b_to_a);
         trx.context_free_actions.emplace_back(action({}, config::null_account_name, "nonce", dp::raw::pack(nonce++)));
         trx.set_reference_block(reference_block_id);
         trx.expiration = cc.head_block_time() + dp::seconds(30);
         trx.max_net_usage_words = 100;
         trx.sign(b_priv_key, chainid);
         trxs.emplace_back(std::move(trx));
         }
         }
      } catch ( const dp::exception& e ) {
         next(e.dynamic_copy_exception());
      }

      push_transactions(std::move(trxs), next);
   }

   void stop_generation() {
      if(!running)
         throw dp::exception(dp::invalid_operation_exception_code);
      timer.cancel();
      running = false;
      ilog("Stopping transaction generation test");
   }

   boost::asio::high_resolution_timer timer{app().get_io_service()};
   bool running{false};

   unsigned timer_timeout;
   unsigned batch;

   action act_a_to_b;
   action act_b_to_a;

   int32_t txn_reference_block_lag;
};

txn_test_gen_plugin::txn_test_gen_plugin() {}
txn_test_gen_plugin::~txn_test_gen_plugin() {}

void txn_test_gen_plugin::set_program_options(options_description&, options_description& cfg) {
   cfg.add_options()
      ("txn-reference-block-lag", bpo::value<int32_t>()->default_value(0), "Lag in number of blocks from the head block when selecting the reference block for transactions (-1 means Last Irreversible Block)")
   ;
}

void txn_test_gen_plugin::plugin_initialize(const variables_map& options) {
   try {
      my.reset( new txn_test_gen_plugin_impl );
      my->txn_reference_block_lag = options.at( "txn-reference-block-lag" ).as<int32_t>();
   } DP_LOG_AND_RETHROW()
}

void txn_test_gen_plugin::plugin_startup() {
   app().get_plugin<http_plugin>().add_api({
      CALL_ASYNC(txn_test_gen, my, create_test_accounts, INVOKE_ASYNC_R_R(my, create_test_accounts, std::string, std::string), 200),
      CALL(txn_test_gen, my, stop_generation, INVOKE_V_V(my, stop_generation), 200),
      CALL(txn_test_gen, my, start_generation, INVOKE_V_R_R_R(my, start_generation, std::string, uint64_t, uint64_t), 200)
   });
}

void txn_test_gen_plugin::plugin_shutdown() {
   try {
      my->stop_generation();
   }
   catch(dp::exception e) {
   }
}

}
