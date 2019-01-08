/**
 *  @file
 *  @copyright defined in dos/LICENSE.txt
 */
#pragma once
#include <dosio/chain/database_utils.hpp>
#include <dosio/chain/authority.hpp>
#include <dosio/chain/block_timestamp.hpp>
#include <dosio/chain/abi_def.hpp>

#include "multi_index_includes.hpp"

namespace dosio { namespace chain {

   class account_object : public chainbase::object<account_object_type, account_object> {
      OBJECT_CTOR(account_object,(code)(abi))

      id_type              id;
      account_name         name;
      uint8_t              vm_type      = 0;
      uint8_t              vm_version   = 0;
      bool                 privileged   = false;

      time_point           last_code_update;
      digest_type          code_version;
      block_timestamp_type creation_date;

      shared_blob    code;
      shared_blob    abi;

      void set_abi( const dosio::chain::abi_def& a ) {
         abi.resize( dp::raw::pack_size( a ) );
         dp::datastream<char*> ds( abi.data(), abi.size() );
         dp::raw::pack( ds, a );
      }

      dosio::chain::abi_def get_abi()const {
         dosio::chain::abi_def a;
         DOS_ASSERT( abi.size() != 0, abi_not_found_exception, "No ABI set on account ${n}", ("n",name) );

         dp::datastream<const char*> ds( abi.data(), abi.size() );
         dp::raw::unpack( ds, a );
         return a;
      }
   };
   using account_id_type = account_object::id_type;

   struct by_name;
   using account_index = chainbase::shared_multi_index_container<
      account_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<account_object, account_object::id_type, &account_object::id>>,
         ordered_unique<tag<by_name>, member<account_object, account_name, &account_object::name>>
      >
   >;

   class account_sequence_object : public chainbase::object<account_sequence_object_type, account_sequence_object>
   {
      OBJECT_CTOR(account_sequence_object);

      id_type      id;
      account_name name;
      uint64_t     recv_sequence = 0;
      uint64_t     auth_sequence = 0;
      uint64_t     code_sequence = 0;
      uint64_t     abi_sequence  = 0;
   };

   struct by_name;
   using account_sequence_index = chainbase::shared_multi_index_container<
      account_sequence_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<account_sequence_object, account_sequence_object::id_type, &account_sequence_object::id>>,
         ordered_unique<tag<by_name>, member<account_sequence_object, account_name, &account_sequence_object::name>>
      >
   >;

} } // dosio::chain

CHAINBASE_SET_INDEX_TYPE(dosio::chain::account_object, dosio::chain::account_index)
CHAINBASE_SET_INDEX_TYPE(dosio::chain::account_sequence_object, dosio::chain::account_sequence_index)


DP_REFLECT(dosio::chain::account_object, (name)(vm_type)(vm_version)(privileged)(last_code_update)(code_version)(creation_date)(code)(abi))
DP_REFLECT(dosio::chain::account_sequence_object, (name)(recv_sequence)(auth_sequence)(code_sequence)(abi_sequence))