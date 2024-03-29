#pragma once
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/elliptic.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/elliptic_r1.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/public_key.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/reflect/reflect.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/reflect/variant.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/static_variant.hpp>

namespace dp { namespace crypto {

   namespace config {
      constexpr const char* private_key_base_prefix = "PVT";
      constexpr const char* private_key_prefix[] = {
         "K1",
         "R1"
      };
   };

   class private_key
   {
      public:
         using storage_type = static_variant<ecc::private_key_shim, r1::private_key_shim>;

         private_key() = default;
         private_key( private_key&& ) = default;
         private_key( const private_key& ) = default;
         private_key& operator= (const private_key& ) = default;

         public_key     get_public_key() const;
         signature      sign( const sha256& digest, bool require_canonical = true ) const;
         sha512         generate_shared_secret( const public_key& pub ) const;

         template< typename KeyType = ecc::private_key_shim >
         static private_key generate() {
            return private_key(storage_type(KeyType::generate()));
         }

         template< typename KeyType = r1::private_key_shim >
         static private_key generate_r1() {
            return private_key(storage_type(KeyType::generate()));
         }

         template< typename KeyType = ecc::private_key_shim >
         static private_key regenerate( const typename KeyType::data_type& data ) {
            return private_key(storage_type(KeyType(data)));
         }

         // serialize to/from string
         explicit private_key(const string& base58str);
         explicit operator string() const;

      private:
         storage_type _storage;

         private_key( storage_type&& other_storage )
         :_storage(forward<storage_type>(other_storage))
         {}

         friend bool operator == ( const private_key& p1, const private_key& p2);
         friend bool operator != ( const private_key& p1, const private_key& p2);
         friend bool operator < ( const private_key& p1, const private_key& p2);
         friend struct reflector<private_key>;
   }; // private_key

} }

namespace dp {
   void to_variant(const crypto::private_key& var,  variant& vo);

   void from_variant(const variant& var, crypto::private_key& vo);
} // namespace dp

DP_REFLECT(dp::crypto::private_key, (_storage) )
