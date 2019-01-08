#pragma once
#include <dp/log/appender.hpp>
#include <dp/log/logger.hpp>
#include <vector>

namespace dp 
{
   class console_appender : public appender 
   {
       public:
            struct color 
            {
                enum type {
                   red,
                   green,
                   brown,
                   blue,
                   magenta,
                   cyan,
                   white,
                   console_default,
                };
            };

            struct stream { enum type { std_out, std_error }; };

            struct level_color 
            {
               level_color( log_level l=log_level::all, 
                            color::type c=color::console_default )
               :level(l),color(c){}

               log_level                         level;
               console_appender::color::type     color;
            };

            struct config 
            {
               config()
               :format( "${timestamp} ${thread_name} ${context} ${file}:${line} ${method} ${level}]  ${message}" ),
                stream(console_appender::stream::std_error),flush(true){}

               dp::string                         format;
               console_appender::stream::type     stream;
               std::vector<level_color>           level_colors;
               bool                               flush;
            };


            console_appender( const variant& args );
            console_appender( const config& cfg );
            console_appender();

            ~console_appender();
            void initialize( boost::asio::io_service& io_service ) {}
            virtual void log( const log_message& m );
            
            void print( const std::string& text_to_print, 
                        color::type text_color = color::console_default );

            void configure( const config& cfg );

       private:
            class impl;
            std::unique_ptr<impl> my;
   };
} // namespace dp

#include <dp/reflect/reflect.hpp>
DP_REFLECT_ENUM( dp::console_appender::stream::type, (std_out)(std_error) )
DP_REFLECT_ENUM( dp::console_appender::color::type, (red)(green)(brown)(blue)(magenta)(cyan)(white)(console_default) )
DP_REFLECT( dp::console_appender::level_color, (level)(color) )
DP_REFLECT( dp::console_appender::config, (format)(stream)(level_colors)(flush) )
