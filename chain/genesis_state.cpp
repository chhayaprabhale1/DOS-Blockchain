#include <chain/include/genesis_state.hpp>
#include <dp/smart_ref_impl.hpp>   // required for gcc in release mode

namespace dosio { namespace chain {

genesis_state::genesis_state() {
   initial_timestamp = dp::time_point::from_iso_string( "2018-06-01T12:00:00" );
   initial_key = dp::variant(dosio_root_key).as<public_key_type>();
}

chain::chain_id_type genesis_state::compute_chain_id() const {
   digest_type::encoder enc;
   dp::raw::pack( enc, *this );
   return chain_id_type{enc.result()};
}

} }
