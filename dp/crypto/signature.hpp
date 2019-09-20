#pragma once
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/static_variant.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/elliptic.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/elliptic_r1.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/reflect/reflect.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/reflect/variant.hpp>

namespace dp { namespace crypto {
   namespace config {
      constexpr const char* signature_base_prefix = "SIN";
      constexpr const char* signature_prefix[] = {
         "K1",
         "R1"
      };
   };

   class signature
   {
      public:
         using storage_type = static_variant<ecc::signature_shim, r1::signature_shim>;

         signature() = default;
         signature( signature&& ) = default;
         signature( const signature& ) = default;
         signature& operator= (const signature& ) = default;

         // serialize to/from string
         explicit signature(const string& base58str);
         explicit operator string() const;

      private:
         storage_type _storage;

         signature( storage_type&& other_storage )
         :_storage(std::forward<storage_type>(other_storage))
         {}

         friend bool operator == ( const signature& p1, const signature& p2);
         friend bool operator != ( const signature& p1, const signature& p2);
         friend bool operator < ( const signature& p1, const signature& p2);
         friend std::size_t hash_value(const signature& b); //not cryptographic; for containers
         friend struct reflector<signature>;
         friend class private_key;
         friend class public_key;
   }; // public_key

} }  // dp::crypto

namespace dp {
   void to_variant(const crypto::signature& var,  variant& vo);

   void from_variant(const variant& var, crypto::signature& vo);
} // namespace dp

DP_REFLECT(dp::crypto::signature, (_storage) )
