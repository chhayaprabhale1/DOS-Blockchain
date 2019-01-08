#pragma once
#include <dp/io/raw.hpp>
#include <dp/reflect/reflect.hpp>
#include <dp/crypto/sha256.hpp>

namespace dp {

   template<typename T>
   dp::sha256 digest( const T& value )
   {
      dp::sha256::encoder enc;
      dp::raw::pack( enc, value );
      return enc.result();
   }
}
