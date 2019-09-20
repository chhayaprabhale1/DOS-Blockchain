#pragma once
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/log/logger.hpp>

namespace dp {
   class path;
   struct appender_config {
      appender_config(const string& name = "",
                      const string& type = "",
                      variant args = variant()) :
        name(name),
        type(type),
        args(dp::move(args)),
        enabled(true)
      {}
      string   name;
      string   type;
      variant  args;
      bool     enabled;
   };

   struct logger_config {
      logger_config(const dp::string& name = ""):name(name),enabled(true),additivity(false){}
      string                           name;
      ostring                          parent;
      /// if not set, then parents level is used.
      dp::optional<log_level>          level;
      bool                             enabled;
      /// if any appenders are sepecified, then parent's appenders are not set.
      bool                             additivity;
      std::vector<string>               appenders;

      logger_config& add_appender( const string& s );
   };

   struct logging_config {
      static logging_config default_config();
      std::vector<string>          includes;
      std::vector<appender_config> appenders;
      std::vector<logger_config>   loggers;
   };

   void configure_logging( const dp::path& log_config );
   bool configure_logging( const logging_config& l );

   void set_thread_name( const string& name );
   const string& get_thread_name();
}

#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/reflect/reflect.hpp>
DP_REFLECT( dp::appender_config, (name)(type)(args)(enabled) )
DP_REFLECT( dp::logger_config, (name)(parent)(level)(enabled)(additivity)(appenders) )
DP_REFLECT( dp::logging_config, (includes)(appenders)(loggers) )
