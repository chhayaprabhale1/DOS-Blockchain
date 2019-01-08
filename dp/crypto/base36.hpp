#pragma once
#include <dp/vector.hpp>
#include <dp/string.hpp>

namespace dp
{
    std::vector<char> from_base36( const dp::string& b36 );
    dp::string to_base36( const std::vector<char>& vec );
    dp::string to_base36( const char* data, size_t len );
}
