#pragma once

#include <dos/wallet_header/wallet_api.hpp>


using namespace std;
using namespace dos::chain;

namespace dos { namespace wallet {

typedef uint16_t transaction_handle_type;

struct wallet_data
{
   vector<char>              cipher_keys; /** encrypted keys */
};

namespace detail {
class soft_wallet_impl;
}

class soft_wallet final : public wallet_api
{
   public:
      soft_wallet( const wallet_data& initial_data );

      ~soft_wallet();

      bool copy_wallet_file( string destination_filename );

      string                            get_wallet_filename() const;

      private_key_type get_private_key( public_key_type pubkey )const override;

      pair<public_key_type,private_key_type>  get_private_key_from_password( string account, string role, string password )const;

      bool    is_new()const;

      bool    is_locked()const override;

      void    lock() override;

      void    unlock(string password) override;

      void    check_password(string password) override;

      void    set_password(string password) override;

      map<public_key_type, private_key_type> list_keys() override;
      
      flat_set<public_key_type> list_public_keys() override;

      bool    load_wallet_file(string wallet_filename = "");

      void    save_wallet_file(string wallet_filename = "");

      void    set_wallet_filename(string wallet_filename);

      bool import_key( string wif_key ) override;

      bool remove_key( string key ) override;

      string create_key( string key_type ) override;

      optional<signature_type> try_sign_digest( const digest_type digest, const public_key_type public_key ) override;

      std::shared_ptr<detail::soft_wallet_impl> my;
      void encrypt_keys();
};

struct plain_keys {
   dp::sha512                            checksum;
   map<public_key_type,private_key_type> keys;
};

} }

DP_REFLECT( dos::wallet::wallet_data, (cipher_keys) )

DP_REFLECT( dos::wallet::plain_keys, (checksum)(keys) )

