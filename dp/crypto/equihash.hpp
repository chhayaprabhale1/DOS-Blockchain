#pragma once
#include <dp/crypto/sha256.hpp>
#include <dp/vector.hpp>

namespace dp { namespace equihash {

   struct proof
   {
      uint32_t n;
      uint32_t k;
      sha256   seed;
      std::vector< uint32_t > inputs;

      bool is_valid() const;

      static proof hash( uint32_t n, uint32_t k, sha256 seed );
   };

} } // dp

DP_REFLECT( dp::equihash::proof, (n)(k)(seed)(inputs) )
