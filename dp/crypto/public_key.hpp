#pragma once
#include <dp/crypto/elliptic.hpp>
#include <dp/crypto/elliptic_r1.hpp>
#include <dp/crypto/signature.hpp>
#include <dp/reflect/reflect.hpp>
#include <dp/reflect/variant.hpp>
#include <dp/static_variant.hpp>

namespace dp { namespace crypto {
   namespace config {
      constexpr const char* public_key_legacy_prefix = "DOS";
      constexpr const char* public_key_base_prefix = "PBK";
      constexpr const char* public_key_prefix[] = {
         "K1",
         "R1"
      };
   };

   class public_key
   {
      public:
         using storage_type = static_variant<ecc::public_key_shim, r1::public_key_shim>;

         public_key() = default;
         public_key( public_key&& ) = default;
         public_key( const public_key& ) = default;
         public_key& operator= (const public_key& ) = default;

         public_key( const signature& c, const sha256& digest, bool check_canonical = true );

         bool valid()const;

         // serialize to/from string
         explicit public_key(const string& base58str);
         explicit operator string() const;

      private:
         storage_type _storage;

         public_key( storage_type&& other_storage )
         :_storage(forward<storage_type>(other_storage))
         {}

         friend std::ostream& operator<< (std::ostream& s, const public_key& k);
         friend bool operator == ( const public_key& p1, const public_key& p2);
         friend bool operator != ( const public_key& p1, const public_key& p2);
         friend bool operator < ( const public_key& p1, const public_key& p2);
         friend struct reflector<public_key>;
         friend class private_key;
   }; // public_key

} }  // dp::crypto

namespace dp {
   void to_variant(const crypto::public_key& var,  variant& vo);

   void from_variant(const variant& var, crypto::public_key& vo);
} // namespace dp

DP_REFLECT(dp::crypto::public_key, (_storage) )