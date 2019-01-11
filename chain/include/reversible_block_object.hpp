
/**
 *  @file
 *  @copyright defined in dos/LICENSE.txt
 */
#pragma once
#include <dosio/chain/types.hpp>
#include <dosio/chain/authority.hpp>
#include <dosio/chain/block_timestamp.hpp>
#include <dosio/chain/contract_types.hpp>

#include "multi_index_includes.hpp"

namespace dosio { namespace chain {

   class reversible_block_object : public chainbase::object<reversible_block_object_type, reversible_block_object> {
      OBJECT_CTOR(reversible_block_object,(packedblock) )

      id_type        id;
      uint32_t       blocknum = 0;
      shared_string  packedblock;

      void set_block( const signed_block_ptr& b ) {
         packedblock.resize( dp::raw::pack_size( *b ) );
         dp::datastream<char*> ds( packedblock.data(), packedblock.size() );
         dp::raw::pack( ds, *b );
      }

      signed_block_ptr get_block()const {
         dp::datastream<const char*> ds( packedblock.data(), packedblock.size() );
         auto result = std::make_shared<signed_block>();
         dp::raw::unpack( ds, *result );
         return result;
      }
   };

   struct by_num;
   using reversible_block_index = chainbase::shared_multi_index_container<
      reversible_block_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<reversible_block_object, reversible_block_object::id_type, &reversible_block_object::id>>,
         ordered_unique<tag<by_num>, member<reversible_block_object, uint32_t, &reversible_block_object::blocknum>>
      >
   >;

} } // dosio::chain

CHAINBASE_SET_INDEX_TYPE(dosio::chain::reversible_block_object, dosio::chain::reversible_block_index)
