#include <chainbase.hpp>
#include <boost/array.hpp>

#include <iostream>

#include <sys/mman.h>

namespace chainbase {

   struct environment_check {
      environment_check() {
         memset( &compiler_version, 0, sizeof( compiler_version ) );
         memcpy( &compiler_version, __VERSION__, std::min<size_t>( strlen(__VERSION__), 256 ) );
#ifndef NDEBUG
         debug = true;
#endif
#ifdef __APPLE__
         apple = true;
#endif
#ifdef WIN32
         windows = true;
#endif
         boost_version = BOOST_VERSION;
      }
      friend bool operator == ( const environment_check& a, const environment_check& b ) {
         return std::make_tuple( a.compiler_version, a.debug, a.apple, a.windows, a.boost_version )
            ==  std::make_tuple( b.compiler_version, b.debug, b.apple, b.windows, b.boost_version );
      }

      boost::array<char,256>  compiler_version;
      bool                    debug = false;
      bool                    apple = false;
      bool                    windows = false;
      uint32_t                boost_version;
   };

   database::database(const bfs::path& dir, open_flags flags, uint64_t shared_file_size, bool allow_dirty ) {
      bool write = flags & database::read_write;

      if (!bfs::exists(dir)) {
         if(!write) BOOST_THROW_EXCEPTION( std::runtime_error( "database file not found at " + dir.native() ) );
      }

      bfs::create_directories(dir);

      _data_dir = dir;
      auto abs_path = bfs::absolute( dir / "shared_memory.bin" );

      if( bfs::exists( abs_path ) )
      {
         if( write )
         {
            auto existing_file_size = bfs::file_size( abs_path );
            if( shared_file_size > existing_file_size )
            {
               if( !bip::managed_mapped_file::grow( abs_path.generic_string().c_str(), shared_file_size - existing_file_size ) )
                  BOOST_THROW_EXCEPTION( std::runtime_error( "could not grow database file to requested size." ) );
            }

            _segment.reset( new bip::managed_mapped_file( bip::open_only,
                                                          abs_path.generic_string().c_str()
                                                          ) );
         } else {
            _segment.reset( new bip::managed_mapped_file( bip::open_read_only,
                                                          abs_path.generic_string().c_str()
                                                          ) );
            _read_only = true;
         }

         auto env = _segment->find< environment_check >( "environment" );
         auto host_env = environment_check();
         if( !env.first || !( *env.first == host_env ) ) {
            std::cerr << "database created by a different compiler, build, boost version, or operating system\n"
                      << "Environment differences (host vs database):"
                      << "\n Compiler Version: \n"
                      <<   "                   " << std::hex;
            for( uint32_t i = 0; i < 256; ++i ) {
               char b = *(host_env.compiler_version.data() + i);
               std::cerr << (uint16_t)b;
            }
            std::cerr << " \"";
            for( uint32_t i = 0; i < 256; ++i ) {
               char b = *(host_env.compiler_version.data() + i);
               if( !b ) break;
               std::cerr << b;
            }
            std::cerr << " \"";
            std::cerr << "\n                   vs\n"
                      <<   "                   ";
            for( uint32_t i = 0; i < 256; ++i ) {
               char b = *(env.first->compiler_version.data() + i);
               std::cerr << (uint16_t)b;
            }
            std::cerr << " \"";
            for( uint32_t i = 0; i < 256; ++i ) {
               char b = *(env.first->compiler_version.data() + i);
               if( !b ) break;
               std::cerr << b;
            }
            std::cerr << " \"" << std::dec;
            std::cerr << "\n Debug: " << host_env.debug << " vs " << env.first->debug
                      << "\n Apple: " << host_env.apple << " vs " << env.first->apple
                      << "\n Windows: " << host_env.windows << " vs " << env.first->windows
                      << "\n Boost Version: " << host_env.boost_version << " vs " << env.first->boost_version
                      << std::endl;

            BOOST_THROW_EXCEPTION( std::runtime_error( "database created by a different compiler, build, boost version, or operating system" ) );
         }
      } else {
         _segment.reset( new bip::managed_mapped_file( bip::create_only,
                                                       abs_path.generic_string().c_str(), shared_file_size,
                                                       0, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH
                                                       ) );
         _segment->find_or_construct< environment_check >( "environment" )();
      }

      abs_path = bfs::absolute( dir / "shared_memory.meta" );

      if( bfs::exists( abs_path ) )
      {
         _meta.reset( new bip::managed_mapped_file( bip::open_only, abs_path.generic_string().c_str()
                                                    ) );

         _rw_manager = _meta->find< read_write_mutex_manager >( "rw_manager" ).first;
         if( !_rw_manager )
            BOOST_THROW_EXCEPTION( std::runtime_error( "could not find read write lock manager" ) );
      }
      else
      {
         _meta.reset( new bip::managed_mapped_file( bip::create_only,
                                                    abs_path.generic_string().c_str(), sizeof( read_write_mutex_manager ) * 2,
                                                    0, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH
                                                    ) );

         _rw_manager = _meta->find_or_construct< read_write_mutex_manager >( "rw_manager" )();
      }

      bool* db_is_dirty   = nullptr;
      bool* meta_is_dirty = nullptr;

      if( write )
      {
         db_is_dirty = _segment->get_segment_manager()->find_or_construct<bool>(_db_dirty_flag_string)(false);
         meta_is_dirty = _meta->get_segment_manager()->find_or_construct<bool>(_db_dirty_flag_string)(false);
      } else {
         db_is_dirty = _segment->get_segment_manager()->find_no_lock<bool>(_db_dirty_flag_string).first;
         meta_is_dirty = _meta->get_segment_manager()->find_no_lock<bool>(_db_dirty_flag_string).first;
      }

      if( db_is_dirty == nullptr || meta_is_dirty == nullptr )
         BOOST_THROW_EXCEPTION( std::runtime_error( "could not find dirty flag in shared memory" ) );

      if( !allow_dirty && *db_is_dirty )
         throw std::runtime_error( "database dirty flag set" );
      if( !allow_dirty && *meta_is_dirty )
         throw std::runtime_error( "database metadata dirty flag set" );

      if( write ) {
         _flock = bip::file_lock( abs_path.generic_string().c_str() );
         if( !_flock.try_lock() )
            BOOST_THROW_EXCEPTION( std::runtime_error( "could not gain write access to the shared memory file" ) );

         *db_is_dirty = *meta_is_dirty = true;
         _msync_database();
      }
   }

   database::~database()
   {
      if(!_read_only) {
         _msync_database();
         *_segment->get_segment_manager()->find<bool>(_db_dirty_flag_string).first = false;
         *_meta->get_segment_manager()->find<bool>(_db_dirty_flag_string).first = false;
         _msync_database();
      }
      _segment.reset();
      _meta.reset();
      _index_list.clear();
      _index_map.clear();
      _data_dir = bfs::path();
   }

   void database::flush() {
      if( _segment )
         _segment->flush();
      if( _meta )
         _meta->flush();
   }

   void database::_msync_database() {
#ifdef _WIN32
#warning Safe database dirty handling not implemented on WIN32
#else
         if(msync(_segment->get_address(), _segment->get_size(), MS_SYNC))
            perror("Failed to msync DB file");
         if(msync(_meta->get_address(), _meta->get_size(), MS_SYNC))
            perror("Failed to msync DB metadata file");
#endif
   }

   void database::set_require_locking( bool enable_require_locking )
   {
#ifdef CHAINBASE_CHECK_LOCKING
      _enable_require_locking = enable_require_locking;
#endif
   }

#ifdef CHAINBASE_CHECK_LOCKING
   void database::require_lock_fail( const char* method, const char* lock_type, const char* tname )const
   {
      std::string err_msg = "database::" + std::string( method ) + " require_" + std::string( lock_type ) + "_lock() failed on type " + std::string( tname );
      std::cerr << err_msg << std::endl;
      BOOST_THROW_EXCEPTION( std::runtime_error( err_msg ) );
   }
#endif

   void database::undo()
   {
      for( auto& item : _index_list )
      {
         item->undo();
      }
   }

   void database::squash()
   {
      for( auto& item : _index_list )
      {
         item->squash();
      }
   }

   void database::commit( int64_t revision )
   {
      for( auto& item : _index_list )
      {
         item->commit( revision );
      }
   }

   void database::undo_all()
   {
      for( auto& item : _index_list )
      {
         item->undo_all();
      }
   }

   database::session database::start_undo_session( bool enabled )
   {
      if( enabled ) {
         vector< std::unique_ptr<abstract_session> > _sub_sessions;
         _sub_sessions.reserve( _index_list.size() );
         for( auto& item : _index_list ) {
            _sub_sessions.push_back( item->start_undo_session( enabled ) );
         }
         return session( std::move( _sub_sessions ) );
      } else {
         return session();
      }
   }

}  // namespace chainbase
