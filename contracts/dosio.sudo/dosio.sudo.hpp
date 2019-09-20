#pragma once

#include <dosiolib/dosio.hpp>

namespace dosio {

   class sudo : public contract {
      public:
         sudo( account_name self ):contract(self){}

         void exec();

   };

} /// namespace dosio
