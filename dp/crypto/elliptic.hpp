#pragma once
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/bigint.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/common.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/openssl.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/sha256.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/sha512.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/fwd.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/array.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/io/raw_fwd.hpp>

namespace dp {

  namespace ecc {
    namespace detail
    {
      class public_key_impl;
      class private_key_impl;
    }

    typedef dp::sha256                  blind_factor_type;
    typedef dp::array<char,33>          commitment_type;
    typedef dp::array<char,33>          public_key_data;
    typedef dp::sha256                  private_key_secret;
    typedef dp::array<char,65>          public_key_point_data; ///< the full non-compressed version of the ECC point
    typedef dp::array<char,72>          signature;
    typedef dp::array<unsigned char,65> compact_signature;
    typedef std::vector<char>           range_proof_type;
    typedef dp::array<char,78>          extended_key_data;
    typedef dp::sha256                  blinded_hash;
    typedef dp::sha256                  blind_signature;

    /**
     *  @class public_key
     *  @brief contains only the public point of an elliptic curve key.
     */
    class public_key
    {
        public:
           public_key();
           public_key(const public_key& k);
           ~public_key();
//           bool verify( const dp::sha256& digest, const signature& sig );
           public_key_data serialize()const;
           public_key_point_data serialize_ecc_point()const;

           operator public_key_data()const { return serialize(); }


           public_key( const public_key_data& v );
           public_key( const public_key_point_data& v );
           public_key( const compact_signature& c, const dp::sha256& digest, bool check_canonical = true );

           public_key child( const dp::sha256& offset )const;

           bool valid()const;
           /** Computes new pubkey = generator * offset + old pubkey ?! */
//           public_key mult( const dp::sha256& offset )const;
           /** Computes new pubkey = regenerate(offset).pubkey + old pubkey
            *                      = offset * G + 1 * old pubkey ?! */
           public_key add( const dp::sha256& offset )const;

           public_key( public_key&& pk );
           public_key& operator=( public_key&& pk );
           public_key& operator=( const public_key& pk );

           inline friend bool operator==( const public_key& a, const public_key& b )
           {
            return a.serialize() == b.serialize();
           }
           inline friend bool operator!=( const public_key& a, const public_key& b )
           {
            return a.serialize() != b.serialize();
           }

           /// Allows to convert current public key object into base58 number.
           std::string to_base58() const;
           static std::string to_base58( const public_key_data &key );
           static public_key from_base58( const std::string& b58 );

           unsigned int fingerprint() const;

        private:
          friend class private_key;
          static public_key from_key_data( const public_key_data& v );
          static bool is_canonical( const compact_signature& c );
          dp::fwd<detail::public_key_impl,33> my;
    };

    /**
     *  @class private_key
     *  @brief an elliptic curve private key.
     */
    class private_key
    {
        public:
           private_key();
           private_key( private_key&& pk );
           private_key( const private_key& pk );
           ~private_key();

           private_key& operator=( private_key&& pk );
           private_key& operator=( const private_key& pk );

           static private_key generate();
           static private_key regenerate( const dp::sha256& secret );

           private_key child( const dp::sha256& offset )const;

           /**
            *  This method of generation enables creating a new private key in a deterministic manner relative to
            *  an initial seed.   A public_key created from the seed can be multiplied by the offset to calculate
            *  the new public key without having to know the private key.
            */
           static private_key generate_from_seed( const dp::sha256& seed, const dp::sha256& offset = dp::sha256() );

           private_key_secret get_secret()const; // get the private key secret

           operator private_key_secret ()const { return get_secret(); }

           /**
            *  Given a public key, calculatse a 512 bit shared secret between that
            *  key and this private key.
            */
           dp::sha512 get_shared_secret( const public_key& pub )const;

//           signature         sign( const dp::sha256& digest )const;
           compact_signature sign_compact( const dp::sha256& digest, bool require_canonical = true )const;
//           bool              verify( const dp::sha256& digest, const signature& sig );

           public_key get_public_key()const;

           inline friend bool operator==( const private_key& a, const private_key& b )
           {
            return a.get_secret() == b.get_secret();
           }
           inline friend bool operator!=( const private_key& a, const private_key& b )
           {
            return a.get_secret() != b.get_secret();
           }
           inline friend bool operator<( const private_key& a, const private_key& b )
           {
            return a.get_secret() < b.get_secret();
           }

           unsigned int fingerprint() const { return get_public_key().fingerprint(); }

        private:
           private_key( EC_KEY* k );
           static dp::sha256 get_secret( const EC_KEY * const k );
           dp::fwd<detail::private_key_impl,32> my;
    };

     struct range_proof_info
     {
         int          exp;
         int          mantissa;
         uint64_t     min_value;
         uint64_t     max_value;
     };

     commitment_type   blind( const blind_factor_type& blind, uint64_t value );
     blind_factor_type blind_sum( const std::vector<blind_factor_type>& blinds, uint32_t non_neg );
     /**  verifies taht commnits + neg_commits + excess == 0 */
     bool            verify_sum( const std::vector<commitment_type>& commits, const std::vector<commitment_type>& neg_commits, int64_t excess );
     bool            verify_range( uint64_t& min_val, uint64_t& max_val, const commitment_type& commit, const range_proof_type& proof );

     range_proof_type range_proof_sign( uint64_t min_value,
                                       const commitment_type& commit,
                                       const blind_factor_type& commit_blind,
                                       const blind_factor_type& nonce,
                                       int8_t base10_exp,
                                       uint8_t min_bits,
                                       uint64_t actual_value
                                     );

     bool            verify_range_proof_rewind( blind_factor_type& blind_out,
                                          uint64_t& value_out,
                                          string& message_out,
                                          const blind_factor_type& nonce,
                                          uint64_t& min_val,
                                          uint64_t& max_val,
                                          commitment_type commit,
                                          const range_proof_type& proof );
     range_proof_info range_get_info( const range_proof_type& proof );

      /**
       * Shims
       */
      struct public_key_shim : public crypto::shim<public_key_data> {
         using crypto::shim<public_key_data>::shim;

         bool valid()const {
            return public_key(_data).valid();
         }
      };

      struct signature_shim : public crypto::shim<compact_signature> {
         using public_key_type = public_key_shim;
         using crypto::shim<compact_signature>::shim;

         public_key_type recover(const sha256& digest, bool check_canonical) const {
            return public_key_type(public_key(_data, digest, check_canonical).serialize());
         }
      };

      struct private_key_shim : public crypto::shim<private_key_secret> {
         using crypto::shim<private_key_secret>::shim;
         using signature_type = signature_shim;
         using public_key_type = public_key_shim;

         signature_type sign( const sha256& digest, bool require_canonical = true ) const
         {
           return signature_type(private_key::regenerate(_data).sign_compact(digest, require_canonical));
         }

         public_key_type get_public_key( ) const
         {
           return public_key_type(private_key::regenerate(_data).get_public_key().serialize());
         }

         sha512 generate_shared_secret( const public_key_type &pub_key ) const
         {
           return private_key::regenerate(_data).get_shared_secret(public_key(pub_key.serialize()));
         }

         static private_key_shim generate()
         {
            return private_key_shim(private_key::generate().get_secret());
         }

      };

  } // namespace ecc
  void to_variant( const ecc::private_key& var,  variant& vo );
  void from_variant( const variant& var,  ecc::private_key& vo );
  void to_variant( const ecc::public_key& var,  variant& vo );
  void from_variant( const variant& var,  ecc::public_key& vo );

  namespace raw
  {
      template<typename Stream>
      void unpack( Stream& s, dp::ecc::public_key& pk)
      {
          ecc::public_key_data ser;
          dp::raw::unpack(s,ser);
          pk = dp::ecc::public_key( ser );
      }

      template<typename Stream>
      void pack( Stream& s, const dp::ecc::public_key& pk)
      {
          dp::raw::pack( s, pk.serialize() );
      }

      template<typename Stream>
      void unpack( Stream& s, dp::ecc::private_key& pk)
      {
          dp::sha256 sec;
          unpack( s, sec );
          pk = ecc::private_key::regenerate(sec);
      }

      template<typename Stream>
      void pack( Stream& s, const dp::ecc::private_key& pk)
      {
          dp::raw::pack( s, pk.get_secret() );
      }

  } // namespace raw

} // namespace dp
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/reflect/reflect.hpp>

DP_REFLECT_TYPENAME( dp::ecc::private_key )
DP_REFLECT_TYPENAME( dp::ecc::public_key )
DP_REFLECT( dp::ecc::range_proof_info, (exp)(mantissa)(min_value)(max_value) )
DP_REFLECT_DERIVED( dp::ecc::public_key_shim, (dp::crypto::shim<dp::ecc::public_key_data>), BOOST_PP_SEQ_NIL )
DP_REFLECT_DERIVED( dp::ecc::signature_shim, (dp::crypto::shim<dp::ecc::compact_signature>), BOOST_PP_SEQ_NIL )
DP_REFLECT_DERIVED( dp::ecc::private_key_shim, (dp::crypto::shim<dp::ecc::private_key_secret>), BOOST_PP_SEQ_NIL )
