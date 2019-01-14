#pragma once

#include <libintl.h>

namespace dosio { namespace client { namespace localize {
   #if !defined(_)
   #define _(str) str
   #endif

   #define localized(str, ...) localized_with_variant((str), dp::mutable_variant_object() __VA_ARGS__ )

   inline auto localized_with_variant( const char* raw_fmt, const dp::variant_object& args) {
      if (raw_fmt != nullptr) {
         try {
            return dp::format_string(::gettext(raw_fmt), args);
         } catch (...) {
         }
         return std::string(raw_fmt);
      }
      return std::string();
   }
}}}
