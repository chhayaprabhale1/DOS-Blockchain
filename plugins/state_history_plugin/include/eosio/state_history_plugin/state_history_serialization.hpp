/**
 *  @file
 *  @copyright defined in dos/LICENSE.txt
 */
#pragma once

#include <dosio/chain/account_object.hpp>
#include <dosio/chain/controller.hpp>
#include <dosio/chain/exceptions.hpp>
#include <dosio/chain/generated_transaction_object.hpp>
#include <dosio/chain/global_property_object.hpp>
#include <dosio/chain/permission_link_object.hpp>
#include <dosio/chain/permission_object.hpp>
#include <dosio/chain/resource_limits.hpp>
#include <dosio/chain/resource_limits_private.hpp>
#include <dosio/chain/trace.hpp>
#include <dosio/chain_plugin/chain_plugin.hpp>
#include <dosio/state_history_plugin/state_history_plugin.hpp>

template <typename T>
struct history_serial_wrapper {
   const chainbase::database& db;
   const T&                   obj;
};

template <typename T>
history_serial_wrapper<T> make_history_serial_wrapper(const chainbase::database& db, const T& obj) {
   return {db, obj};
}

template <typename P, typename T>
struct history_context_wrapper {
   const chainbase::database& db;
   const P&                   context;
   const T&                   obj;
};

template <typename P, typename T>
history_context_wrapper<P, T> make_history_context_wrapper(const chainbase::database& db, P& context, const T& obj) {
   return {db, context, obj};
}

namespace dp {

template <typename T>
const T& as_type(const T& x) {
   return x;
}

template <typename ST, typename T>
datastream<ST>& history_serialize_container(datastream<ST>& ds, const chainbase::database& db, const T& v) {
   dp::raw::pack(ds, unsigned_int(v.size()));
   for (auto& x : v)
      ds << make_history_serial_wrapper(db, x);
   return ds;
}

template <typename ST, typename T>
datastream<ST>& history_serialize_container(datastream<ST>& ds, const chainbase::database& db,
                                            const std::vector<std::shared_ptr<T>>& v) {
   dp::raw::pack(ds, unsigned_int(v.size()));
   for (auto& x : v) {
      DOS_ASSERT(!!x, dosio::chain::plugin_exception, "null inside container");
      ds << make_history_serial_wrapper(db, *x);
   }
   return ds;
}

template <typename ST, typename T>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_big_vector_wrapper<T>& obj) {
   DP_ASSERT(obj.obj.size() <= 1024 * 1024 * 1024);
   dp::raw::pack(ds, unsigned_int((uint32_t)obj.obj.size()));
   for (auto& x : obj.obj)
      dp::raw::pack(ds, x);
   return ds;
}

template <typename ST>
void history_pack_big_bytes(datastream<ST>& ds, const dosio::chain::bytes& v) {
   DP_ASSERT(v.size() <= 1024 * 1024 * 1024);
   dp::raw::pack(ds, unsigned_int((uint32_t)v.size()));
   if (v.size())
      ds.write(&v.front(), (uint32_t)v.size());
}

template <typename ST>
void history_pack_big_bytes(datastream<ST>& ds, const dp::optional<dosio::chain::bytes>& v) {
   dp::raw::pack(ds, v.valid());
   if (v)
      history_pack_big_bytes(ds, *v);
}

template <typename ST, typename T>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<std::vector<T>>& obj) {
   return history_serialize_container(ds, obj.db, obj.obj);
}

template <typename ST, typename First, typename Second>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<std::pair<First, Second>>& obj) {
   dp::raw::pack(ds, obj.obj.first);
   dp::raw::pack(ds, obj.obj.second);
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::account_object>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.name.value));
   dp::raw::pack(ds, as_type<uint8_t>(obj.obj.vm_type));
   dp::raw::pack(ds, as_type<uint8_t>(obj.obj.vm_version));
   dp::raw::pack(ds, as_type<bool>(obj.obj.privileged));
   dp::raw::pack(ds, as_type<dp::time_point>(obj.obj.last_code_update));
   dp::raw::pack(ds, as_type<dosio::chain::digest_type>(obj.obj.code_version));
   dp::raw::pack(ds, as_type<dosio::chain::block_timestamp_type>(obj.obj.creation_date));
   dp::raw::pack(ds, as_type<dosio::chain::shared_string>(obj.obj.code));
   dp::raw::pack(ds, as_type<dosio::chain::shared_string>(obj.obj.abi));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::table_id_object>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.code.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.scope.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.table.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.payer.value));
   return ds;
}

template <typename ST>
datastream<ST>&
operator<<(datastream<ST>&                                                                                     ds,
           const history_context_wrapper<const dosio::chain::table_id_object, dosio::chain::key_value_object>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(obj.context.code.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.context.scope.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.context.table.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.primary_key));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.payer.value));
   dp::raw::pack(ds, as_type<dosio::chain::shared_string>(obj.obj.value));
   return ds;
}

template <typename ST, typename T>
void serialize_secondary_index_data(datastream<ST>& ds, const T& obj) {
   dp::raw::pack(ds, obj);
}

template <typename ST>
void serialize_secondary_index_data(datastream<ST>& ds, const float64_t& obj) {
   uint64_t i;
   memcpy(&i, &obj, sizeof(i));
   dp::raw::pack(ds, i);
}

template <typename ST>
void serialize_secondary_index_data(datastream<ST>& ds, const float128_t& obj) {
   __uint128_t i;
   memcpy(&i, &obj, sizeof(i));
   dp::raw::pack(ds, i);
}

template <typename ST>
void serialize_secondary_index_data(datastream<ST>& ds, const dosio::chain::key256_t& obj) {
   auto rev = [&](__uint128_t x) {
      char* ch = reinterpret_cast<char*>(&x);
      std::reverse(ch, ch + sizeof(x));
      return x;
   };
   dp::raw::pack(ds, rev(obj[0]));
   dp::raw::pack(ds, rev(obj[1]));
}

template <typename ST, typename T>
datastream<ST>& serialize_secondary_index(datastream<ST>& ds, const dosio::chain::table_id_object& context,
                                          const T& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(context.code.value));
   dp::raw::pack(ds, as_type<uint64_t>(context.scope.value));
   dp::raw::pack(ds, as_type<uint64_t>(context.table.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.primary_key));
   dp::raw::pack(ds, as_type<uint64_t>(obj.payer.value));
   serialize_secondary_index_data(ds, obj.secondary_key);
   return ds;
}

template <typename ST>
datastream<ST>&
operator<<(datastream<ST>&                                                                                   ds,
           const history_context_wrapper<const dosio::chain::table_id_object, dosio::chain::index64_object>& obj) {
   return serialize_secondary_index(ds, obj.context, obj.obj);
}

template <typename ST>
datastream<ST>&
operator<<(datastream<ST>&                                                                                    ds,
           const history_context_wrapper<const dosio::chain::table_id_object, dosio::chain::index128_object>& obj) {
   return serialize_secondary_index(ds, obj.context, obj.obj);
}

template <typename ST>
datastream<ST>&
operator<<(datastream<ST>&                                                                                    ds,
           const history_context_wrapper<const dosio::chain::table_id_object, dosio::chain::index256_object>& obj) {
   return serialize_secondary_index(ds, obj.context, obj.obj);
}

template <typename ST>
datastream<ST>&
operator<<(datastream<ST>&                                                                                        ds,
           const history_context_wrapper<const dosio::chain::table_id_object, dosio::chain::index_double_object>& obj) {
   return serialize_secondary_index(ds, obj.context, obj.obj);
}

template <typename ST>
datastream<ST>& operator<<(
    datastream<ST>&                                                                                             ds,
    const history_context_wrapper<const dosio::chain::table_id_object, dosio::chain::index_long_double_object>& obj) {
   return serialize_secondary_index(ds, obj.context, obj.obj);
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::producer_key>& obj) {
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.producer_name.value));
   dp::raw::pack(ds, as_type<dosio::chain::public_key_type>(obj.obj.block_signing_key));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>&                                                            ds,
                           const history_serial_wrapper<dosio::chain::shared_producer_schedule_type>& obj) {
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.version));
   history_serialize_container(ds, obj.db,
                               as_type<dosio::chain::shared_vector<dosio::chain::producer_key>>(obj.obj.producers));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::chain_config>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.max_block_net_usage));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.target_block_net_usage_pct));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.max_transaction_net_usage));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.base_per_transaction_net_usage));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.net_usage_leeway));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.context_free_discount_net_usage_num));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.context_free_discount_net_usage_den));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.max_block_cpu_usage));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.target_block_cpu_usage_pct));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.max_transaction_cpu_usage));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.min_transaction_cpu_usage));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.max_transaction_lifetime));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.deferred_trx_expiration_window));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.max_transaction_delay));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.max_inline_action_size));
   dp::raw::pack(ds, as_type<uint16_t>(obj.obj.max_inline_action_depth));
   dp::raw::pack(ds, as_type<uint16_t>(obj.obj.max_authority_depth));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>&                                                     ds,
                           const history_serial_wrapper<dosio::chain::global_property_object>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<optional<dosio::chain::block_num_type>>(obj.obj.proposed_schedule_block_num));
   dp::raw::pack(ds, make_history_serial_wrapper(
                         obj.db, as_type<dosio::chain::shared_producer_schedule_type>(obj.obj.proposed_schedule)));
   dp::raw::pack(ds, make_history_serial_wrapper(obj.db, as_type<dosio::chain::chain_config>(obj.obj.configuration)));

   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>&                                                           ds,
                           const history_serial_wrapper<dosio::chain::generated_transaction_object>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.sender.value));
   dp::raw::pack(ds, as_type<__uint128_t>(obj.obj.sender_id));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.payer.value));
   dp::raw::pack(ds, as_type<dosio::chain::transaction_id_type>(obj.obj.trx_id));
   dp::raw::pack(ds, as_type<dosio::chain::shared_string>(obj.obj.packed_trx));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::key_weight>& obj) {
   dp::raw::pack(ds, as_type<dosio::chain::public_key_type>(obj.obj.key));
   dp::raw::pack(ds, as_type<uint16_t>(obj.obj.weight));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::permission_level>& obj) {
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.actor.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.permission.value));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>&                                                      ds,
                           const history_serial_wrapper<dosio::chain::permission_level_weight>& obj) {
   dp::raw::pack(ds, make_history_serial_wrapper(obj.db, as_type<dosio::chain::permission_level>(obj.obj.permission)));
   dp::raw::pack(ds, as_type<uint16_t>(obj.obj.weight));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::wait_weight>& obj) {
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.wait_sec));
   dp::raw::pack(ds, as_type<uint16_t>(obj.obj.weight));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::shared_authority>& obj) {
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.threshold));
   history_serialize_container(ds, obj.db, obj.obj.keys);
   history_serialize_container(ds, obj.db, obj.obj.accounts);
   history_serialize_container(ds, obj.db, obj.obj.waits);
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::permission_object>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.owner.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.name.value));
   if (obj.obj.parent._id) {
      auto&       index  = obj.db.get_index<dosio::chain::permission_index>();
      const auto* parent = index.find(obj.obj.parent);
      if (!parent) {
         auto& undo = index.stack().back();
         auto  it   = undo.removed_values.find(obj.obj.parent);
         DOS_ASSERT(it != undo.removed_values.end(), dosio::chain::plugin_exception,
                    "can not find parent of permission_object");
         parent = &it->second;
      }
      dp::raw::pack(ds, as_type<uint64_t>(parent->name.value));
   } else {
      dp::raw::pack(ds, as_type<uint64_t>(0));
   }
   dp::raw::pack(ds, as_type<dp::time_point>(obj.obj.last_updated));
   dp::raw::pack(ds, make_history_serial_wrapper(obj.db, as_type<dosio::chain::shared_authority>(obj.obj.auth)));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>&                                                     ds,
                           const history_serial_wrapper<dosio::chain::permission_link_object>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.account.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.code.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.message_type.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.required_permission.value));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>&                                                                      ds,
                           const history_serial_wrapper<dosio::chain::resource_limits::resource_limits_object>& obj) {
   DOS_ASSERT(!obj.obj.pending, dosio::chain::plugin_exception,
              "accepted_block sent while resource_limits_object in pending state");
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.owner.value));
   dp::raw::pack(ds, as_type<int64_t>(obj.obj.net_weight));
   dp::raw::pack(ds, as_type<int64_t>(obj.obj.cpu_weight));
   dp::raw::pack(ds, as_type<int64_t>(obj.obj.ram_bytes));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>&                                                                 ds,
                           const history_serial_wrapper<dosio::chain::resource_limits::usage_accumulator>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.last_ordinal));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.value_ex));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.consumed));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>&                                                                     ds,
                           const history_serial_wrapper<dosio::chain::resource_limits::resource_usage_object>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.owner.value));
   dp::raw::pack(ds, make_history_serial_wrapper(
                         obj.db, as_type<dosio::chain::resource_limits::usage_accumulator>(obj.obj.net_usage)));
   dp::raw::pack(ds, make_history_serial_wrapper(
                         obj.db, as_type<dosio::chain::resource_limits::usage_accumulator>(obj.obj.cpu_usage)));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.ram_usage));
   return ds;
}

template <typename ST>
datastream<ST>&
operator<<(datastream<ST>&                                                                            ds,
           const history_serial_wrapper<dosio::chain::resource_limits::resource_limits_state_object>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, make_history_serial_wrapper(obj.db, as_type<dosio::chain::resource_limits::usage_accumulator>(
                                                             obj.obj.average_block_net_usage)));
   dp::raw::pack(ds, make_history_serial_wrapper(obj.db, as_type<dosio::chain::resource_limits::usage_accumulator>(
                                                             obj.obj.average_block_cpu_usage)));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.total_net_weight));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.total_cpu_weight));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.total_ram_bytes));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.virtual_net_limit));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.virtual_cpu_limit));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>&                                                     ds,
                           const history_serial_wrapper<dosio::chain::resource_limits::ratio>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.numerator));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.denominator));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>&                                                                        ds,
                           const history_serial_wrapper<dosio::chain::resource_limits::elastic_limit_parameters>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.target));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.max));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.periods));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.max_multiplier));
   dp::raw::pack(
       ds, make_history_serial_wrapper(obj.db, as_type<dosio::chain::resource_limits::ratio>(obj.obj.contract_rate)));
   dp::raw::pack(
       ds, make_history_serial_wrapper(obj.db, as_type<dosio::chain::resource_limits::ratio>(obj.obj.expand_rate)));
   return ds;
}

template <typename ST>
datastream<ST>&
operator<<(datastream<ST>&                                                                             ds,
           const history_serial_wrapper<dosio::chain::resource_limits::resource_limits_config_object>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(
       ds, make_history_serial_wrapper(
               obj.db, as_type<dosio::chain::resource_limits::elastic_limit_parameters>(obj.obj.cpu_limit_parameters)));
   dp::raw::pack(
       ds, make_history_serial_wrapper(
               obj.db, as_type<dosio::chain::resource_limits::elastic_limit_parameters>(obj.obj.net_limit_parameters)));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.account_cpu_usage_average_window));
   dp::raw::pack(ds, as_type<uint32_t>(obj.obj.account_net_usage_average_window));
   return ds;
};

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::action>& obj) {
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.account.value));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.name.value));
   history_serialize_container(ds, obj.db, as_type<std::vector<dosio::chain::permission_level>>(obj.obj.authorization));
   dp::raw::pack(ds, as_type<dosio::bytes>(obj.obj.data));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::action_receipt>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.receiver.value));
   dp::raw::pack(ds, as_type<dosio::chain::digest_type>(obj.obj.act_digest));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.global_sequence));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.recv_sequence));
   history_serialize_container(ds, obj.db, as_type<flat_map<dosio::name, uint64_t>>(obj.obj.auth_sequence));
   dp::raw::pack(ds, as_type<dp::unsigned_int>(obj.obj.code_sequence));
   dp::raw::pack(ds, as_type<dp::unsigned_int>(obj.obj.abi_sequence));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::account_delta>& obj) {
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.account.value));
   dp::raw::pack(ds, as_type<int64_t>(obj.obj.delta));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::action_trace>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, make_history_serial_wrapper(obj.db, as_type<dosio::chain::action_receipt>(obj.obj.receipt)));
   dp::raw::pack(ds, make_history_serial_wrapper(obj.db, as_type<dosio::chain::action>(obj.obj.act)));
   dp::raw::pack(ds, as_type<bool>(obj.obj.context_free));
   dp::raw::pack(ds, as_type<int64_t>(obj.obj.elapsed.count()));
   dp::raw::pack(ds, as_type<std::string>(obj.obj.console));
   history_serialize_container(ds, obj.db, as_type<flat_set<dosio::chain::account_delta>>(obj.obj.account_ram_deltas));

   dp::optional<std::string> e;
   if (obj.obj.except)
      e = obj.obj.except->to_string();
   dp::raw::pack(ds, as_type<dp::optional<std::string>>(e));

   history_serialize_container(ds, obj.db, as_type<std::vector<dosio::chain::action_trace>>(obj.obj.inline_traces));
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>&                                                          ds,
                           const history_context_wrapper<uint8_t, dosio::chain::transaction_trace>& obj) {
   dp::raw::pack(ds, dp::unsigned_int(0));
   dp::raw::pack(ds, as_type<dosio::chain::transaction_id_type>(obj.obj.id));
   if (obj.obj.receipt) {
      if (obj.obj.failed_dtrx_trace &&
          obj.obj.receipt->status.value == dosio::chain::transaction_receipt_header::soft_fail)
         dp::raw::pack(ds, uint8_t(dosio::chain::transaction_receipt_header::executed));
      else
         dp::raw::pack(ds, as_type<uint8_t>(obj.obj.receipt->status.value));
      dp::raw::pack(ds, as_type<uint32_t>(obj.obj.receipt->cpu_usage_us));
      dp::raw::pack(ds, as_type<dp::unsigned_int>(obj.obj.receipt->net_usage_words));
   } else {
      dp::raw::pack(ds, uint8_t(obj.context));
      dp::raw::pack(ds, uint32_t(0));
      dp::raw::pack(ds, dp::unsigned_int(0));
   }
   dp::raw::pack(ds, as_type<int64_t>(obj.obj.elapsed.count()));
   dp::raw::pack(ds, as_type<uint64_t>(obj.obj.net_usage));
   dp::raw::pack(ds, as_type<bool>(obj.obj.scheduled));
   history_serialize_container(ds, obj.db, as_type<std::vector<dosio::chain::action_trace>>(obj.obj.action_traces));

   dp::optional<std::string> e;
   if (obj.obj.except)
      e = obj.obj.except->to_string();
   dp::raw::pack(ds, as_type<dp::optional<std::string>>(e));

   dp::raw::pack(ds, bool(obj.obj.failed_dtrx_trace));
   if (obj.obj.failed_dtrx_trace) {
      uint8_t stat = dosio::chain::transaction_receipt_header::hard_fail;
      if (obj.obj.receipt && obj.obj.receipt->status.value == dosio::chain::transaction_receipt_header::soft_fail)
         stat = dosio::chain::transaction_receipt_header::soft_fail;
      dp::raw::pack(ds, make_history_context_wrapper(obj.db, stat, *obj.obj.failed_dtrx_trace));
   }

   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const history_serial_wrapper<dosio::chain::transaction_trace>& obj) {
   uint8_t stat = dosio::chain::transaction_receipt_header::hard_fail;
   ds << make_history_context_wrapper(obj.db, stat, obj.obj);
   return ds;
}

template <typename ST>
datastream<ST>& operator<<(datastream<ST>& ds, const dosio::get_blocks_result_v0& obj) {
   dp::raw::pack(ds, obj.head);
   dp::raw::pack(ds, obj.last_irreversible);
   dp::raw::pack(ds, obj.this_block);
   dp::raw::pack(ds, obj.prev_block);
   history_pack_big_bytes(ds, obj.block);
   history_pack_big_bytes(ds, obj.traces);
   history_pack_big_bytes(ds, obj.deltas);
   return ds;
}

} // namespace dp
