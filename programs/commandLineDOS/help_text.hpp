#pragma once

namespace dosio { namespace client { namespace help {
   bool print_recognized_errors(const dp::exception& e, const bool verbose_errors);
   bool print_help_text(const dp::exception& e);
}}}
