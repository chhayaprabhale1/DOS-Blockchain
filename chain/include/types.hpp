#pragma once
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/chain/include/name.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/chain/include/chain_id_type.hpp>

#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/chainbase/chainbase.hpp>

#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/container/flat_fwd.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/io/varint.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/io/enum_type.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/sha224.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/optional.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/safe.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/container/flat.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/string.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/io/raw.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/static_variant.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/smart_ref_fwd.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/ripemd160.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/fixed_string.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/private_key.hpp>

#include <memory>
#include <vector>
#include <deque>
#include <cstdint>

#define OBJECT_CTOR1(NAME) \
    NAME() = delete; \
    public: \
    template<typename Constructor, typename Allocator> \
    NAME(Constructor&& c, chainbase::allocator<Allocator>) \
    { c(*this); }
#define OBJECT_CTOR2_MACRO(x, y, field) ,field(a)
#define OBJECT_CTOR2(NAME, FIELDS) \
    NAME() = delete; \
    public: \
    template<typename Constructor, typename Allocator> \
    NAME(Constructor&& c, chainbase::allocator<Allocator> a) \
    : id(0) BOOST_PP_SEQ_FOR_EACH(OBJECT_CTOR2_MACRO, _, FIELDS) \
    { c(*this); }
#define OBJECT_CTOR(...) BOOST_PP_OVERLOAD(OBJECT_CTOR, __VA_ARGS__)(__VA_ARGS__)

#define _V(n, v)  dp::mutable_variant_object(n, v)

namespace dosio { namespace chain {
   using                               std::map;
   using                               std::vector;
   using                               std::unordered_map;
   using                               std::string;
   using                               std::deque;
   using                               std::shared_ptr;
   using                               std::weak_ptr;
   using                               std::unique_ptr;
   using                               std::set;
   using                               std::pair;
   using                               std::make_pair;
   using                               std::enable_shared_from_this;
   using                               std::tie;
   using                               std::move;
   using                               std::forward;
   using                               std::to_string;
   using                               std::all_of;

   using                               dp::path;
   using                               dp::smart_ref;
   using                               dp::variant_object;
   using                               dp::variant;
   using                               dp::enum_type;
   using                               dp::optional;
   using                               dp::unsigned_int;
   using                               dp::signed_int;
   using                               dp::time_point_sec;
   using                               dp::time_point;
   using                               dp::safe;
   using                               dp::flat_map;
   using                               dp::flat_set;
   using                               dp::static_variant;
   using                               dp::ecc::range_proof_type;
   using                               dp::ecc::range_proof_info;
   using                               dp::ecc::commitment_type;

   using public_key_type  = dp::crypto::public_key;
   using private_key_type = dp::crypto::private_key;
   using signature_type   = dp::crypto::signature;

   struct void_t{};

   using chainbase::allocator;
   using shared_string = boost::interprocess::basic_string<char, std::char_traits<char>, allocator<char>>;
   template<typename T>
   using shared_vector = boost::interprocess::vector<T, allocator<T>>;
   template<typename T>
   using shared_set = boost::interprocess::set<T, std::less<T>, allocator<T>>;

   /**
    * For bugs in boost interprocess we moved our blob data to shared_string
    * this wrapper allows us to continue that while also having a type-level distinction for
    * serialization and to/from variant
    */
   class shared_blob : public shared_string {
      public:
         shared_blob() = default;

         template <typename InputIterator>
         shared_blob(InputIterator f, InputIterator l, const allocator_type& a)
         :shared_string(f,l,a)
         {}

         shared_blob(const allocator_type& a)
         :shared_string(a)
         {}
   };

   using action_name      = name;
   using scope_name       = name;
   using account_name     = name;
   using permission_name  = name;
   using table_name       = name;


   /**
    * List all object types from all namespaces here so they can
    * be easily reflected and displayed in debug output.  If a 3rd party
    * wants to extend the core code then they will have to change the
    * packed_object::type field from enum_type to uint16 to avoid
    * warnings when converting packed_objects to/from json.
    *
    * UNUSED_ enums can be taken for new purposes but otherwise the offsets
    * in this enumeration are potentially shared_memory breaking
    */
   enum object_type
   {
      null_object_type = 0,
      account_object_type,
      account_sequence_object_type,
      permission_object_type,
      permission_usage_object_type,
      permission_link_object_type,
      UNUSED_action_code_object_type,
      key_value_object_type,
      index64_object_type,
      index128_object_type,
      index256_object_type,
      index_double_object_type,
      index_long_double_object_type,
      global_property_object_type,
      dynamic_global_property_object_type,
      block_summary_object_type,
      transaction_object_type,
      generated_transaction_object_type,
      producer_object_type,
      UNUSED_chain_property_object_type,
      account_control_history_object_type,     ///< Defined by history_plugin
      UNUSED_account_transaction_history_object_type,
      UNUSED_transaction_history_object_type,
      public_key_history_object_type,          ///< Defined by history_plugin
      UNUSED_balance_object_type,
      UNUSED_staked_balance_object_type,
      UNUSED_producer_votes_object_type,
      UNUSED_producer_schedule_object_type,
      UNUSED_proxy_vote_object_type,
      UNUSED_scope_sequence_object_type,
      table_id_object_type,
      resource_limits_object_type,
      resource_usage_object_type,
      resource_limits_state_object_type,
      resource_limits_config_object_type,
      account_history_object_type,              ///< Defined by history_plugin
      action_history_object_type,               ///< Defined by history_plugin
      reversible_block_object_type,
      OBJECT_TYPE_COUNT ///< Sentry value which contains the number of different object types
   };

   class account_object;
   class producer_object;

   using block_id_type       = dp::sha256;
   using checksum_type       = dp::sha256;
   using checksum256_type    = dp::sha256;
   using checksum512_type    = dp::sha512;
   using checksum160_type    = dp::ripemd160;
   using transaction_id_type = checksum_type;
   using digest_type         = checksum_type;
   using weight_type         = uint16_t;
   using block_num_type      = uint32_t;
   using share_type          = int64_t;
   using int128_t            = __int128;
   using uint128_t           = unsigned __int128;
   using bytes               = vector<char>;


   /**
    *  Extentions are prefixed with type and are a buffer that can be
    *  interpreted by code that is aware and ignored by unaware code.
    */
   typedef vector<std::pair<uint16_t,vector<char>>> extensions_type;


} }

DP_REFLECT( chain::void_t, )
