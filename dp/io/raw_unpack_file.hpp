#pragma once
#include <dp/io/raw.hpp>
#include <dp/interprocess/file_mapping.hpp>
#include <dp/filesystem.hpp>
#include <dp/exception/exception.hpp>

namespace dp
{
    namespace raw
    {
        template<typename T>
        void unpack_file( const dp::path& filename, T& obj )
        {
           try {
               dp::file_mapping fmap( filename.generic_string().c_str(), dp::read_only);
               dp::mapped_region mapr( fmap, dp::read_only, 0, dp::file_size(filename) );
               auto cs  = (const char*)mapr.get_address();

               dp::datastream<const char*> ds( cs, mapr.get_size() );
               dp::raw::unpack(ds,obj);
           } DP_RETHROW_EXCEPTIONS( info, "unpacking file ${file}", ("file",filename) );
        }
   }
}
