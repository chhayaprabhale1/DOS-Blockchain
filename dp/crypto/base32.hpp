#pragma once
#include <dp/vector.hpp>
#include <dp/string.hpp>

namespace dp
{
    std::vector<char> from_base32( const dp::string& b32 );
    dp::string to_base32( const std::vector<char>& vec );
    dp::string to_base32( const char* data, size_t len );
}
