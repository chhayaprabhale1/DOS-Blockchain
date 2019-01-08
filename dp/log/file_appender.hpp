#pragma once

#include <dp/filesystem.hpp>
#include <dp/log/appender.hpp>
#include <dp/log/logger.hpp>
#include <dp/time.hpp>

namespace dp {

class file_appender : public appender {
    public:
         struct config {
            config( const dp::path& p = "log.txt" );

            dp::string                         format;
            dp::path                           filename;
            bool                               flush = true;
            bool                               rotate = false;
            microseconds                       rotation_interval;
            microseconds                       rotation_limit;
         };
         file_appender( const variant& args );
         ~file_appender();
         virtual void log( const log_message& m )override;

      private:
         class impl;
         dp::shared_ptr<impl> my;
   };
} // namespace dp

#include <dp/reflect/reflect.hpp>
DP_REFLECT( dp::file_appender::config,
            (format)(filename)(flush)(rotate)(rotation_interval)(rotation_limit) )
