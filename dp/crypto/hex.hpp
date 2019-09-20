#pragma once
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/string.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/utility.hpp>
#include <vector>

namespace dp {
    uint8_t from_hex( char c );
    dp::string to_hex( const char* d, uint32_t s );
    std::string to_hex( const std::vector<char>& data );

    /**
     *  @return the number of bytes decoded
     */
    size_t from_hex( const dp::string& hex_str, char* out_data, size_t out_data_len );
} 
