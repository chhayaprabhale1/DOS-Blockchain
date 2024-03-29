#pragma once
#include <string>

namespace dp {
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
inline std::string base64_encode(char const* bytes_to_encode, unsigned int in_len) { return base64_encode( (unsigned char const*)bytes_to_encode, in_len); }
std::string base64_encode( const std::string& enc );
std::string base64_decode( const std::string& encoded_string);
}  // namespace dp
