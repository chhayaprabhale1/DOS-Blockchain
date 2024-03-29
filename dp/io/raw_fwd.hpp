#pragma once
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/container/flat_fwd.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/container/deque_fwd.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/io/varint.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/array.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/safe.hpp>
#include <deque>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <set>

#define MAX_NUM_ARRAY_ELEMENTS (1024*1024)
#define MAX_SIZE_OF_BYTE_ARRAYS (20*1024*1024)

namespace dp {
   class time_point;
   class time_point_sec;
   class variant;
   class variant_object;
   class path;
   template<typename... Types> class static_variant;

   template<typename IntType, typename EnumType> class enum_type;
   namespace ip { class endpoint; }

   namespace ecc { class public_key; class private_key; }
   template<typename Storage> class fixed_string;

   namespace raw {
    template<typename T>
    constexpr bool is_trivial_array = std::is_scalar<T>::value == true && std::is_pointer<T>::value == false;

    template<typename T>
    inline size_t pack_size(  const T& v );

    template<typename Stream, typename Storage> inline void pack( Stream& s, const dp::fixed_string<Storage>& u );
    template<typename Stream, typename Storage> inline void unpack( Stream& s, dp::fixed_string<Storage>& u );

    template<typename Stream, typename IntType, typename EnumType>
    inline void pack( Stream& s, const dp::enum_type<IntType,EnumType>& tp );
    template<typename Stream, typename IntType, typename EnumType>
    inline void unpack( Stream& s, dp::enum_type<IntType,EnumType>& tp );



    template<typename Stream, typename T> inline void pack( Stream& s, const std::set<T>& value );
    template<typename Stream, typename T> inline void unpack( Stream& s, std::set<T>& value );
    template<typename Stream, typename T> inline void pack( Stream& s, const std::unordered_set<T>& value );
    template<typename Stream, typename T> inline void unpack( Stream& s, std::unordered_set<T>& value );

    template<typename Stream, typename... T> void pack( Stream& s, const static_variant<T...>& sv );
    template<typename Stream, typename... T> void unpack( Stream& s, static_variant<T...>& sv );

    template<typename Stream, typename T> inline void pack( Stream& s, const flat_set<T>& value );
    template<typename Stream, typename T> inline void unpack( Stream& s, flat_set<T>& value );

    template<typename Stream, typename T> inline void pack( Stream& s, const std::deque<T>& value );
    template<typename Stream, typename T> inline void unpack( Stream& s, std::deque<T>& value );

    template<typename Stream, typename K, typename V> inline void pack( Stream& s, const std::unordered_map<K,V>& value );
    template<typename Stream, typename K, typename V> inline void unpack( Stream& s, std::unordered_map<K,V>& value );

    template<typename Stream, typename K, typename V> inline void pack( Stream& s, const std::map<K,V>& value );
    template<typename Stream, typename K, typename V> inline void unpack( Stream& s, std::map<K,V>& value );

    template<typename Stream, typename K, typename... V> inline void pack( Stream& s, const flat_map<K,V...>& value );
    template<typename Stream, typename K, typename V, typename... A> inline void unpack( Stream& s, flat_map<K,V,A...>& value );

    template<typename Stream, typename K, typename V> inline void pack( Stream& s, const std::pair<K,V>& value );
    template<typename Stream, typename K, typename V> inline void unpack( Stream& s, std::pair<K,V>& value );

    template<typename Stream, typename T, std::size_t S> inline auto pack( Stream& s, const std::array<T,S>& value ) -> std::enable_if_t<is_trivial_array<T>>;
    template<typename Stream, typename T, std::size_t S> inline auto pack( Stream& s, const std::array<T,S>& value ) -> std::enable_if_t<!is_trivial_array<T>>;
    template<typename Stream, typename T, std::size_t S> inline auto unpack( Stream& s, std::array<T,S>& value ) -> std::enable_if_t<is_trivial_array<T>>;
    template<typename Stream, typename T, std::size_t S> inline auto unpack( Stream& s, std::array<T,S>& value ) -> std::enable_if_t<!is_trivial_array<T>>;

    template<typename Stream> inline void pack( Stream& s, const variant_object& v );
    template<typename Stream> inline void unpack( Stream& s, variant_object& v );
    template<typename Stream> inline void pack( Stream& s, const variant& v );
    template<typename Stream> inline void unpack( Stream& s, variant& v );

    template<typename Stream> inline void pack( Stream& s, const path& v );
    template<typename Stream> inline void unpack( Stream& s, path& v );
    template<typename Stream> inline void pack( Stream& s, const ip::endpoint& v );
    template<typename Stream> inline void unpack( Stream& s, ip::endpoint& v );


    template<typename Stream, typename T> void unpack( Stream& s, dp::optional<T>& v );
    template<typename Stream, typename T> void unpack( Stream& s, const T& v );
    template<typename Stream, typename T> void pack( Stream& s, const dp::optional<T>& v );
    template<typename Stream, typename T> void pack( Stream& s, const safe<T>& v );
    template<typename Stream, typename T> void unpack( Stream& s, dp::safe<T>& v );

    template<typename Stream> void unpack( Stream& s, time_point& );
    template<typename Stream> void pack( Stream& s, const time_point& );
    template<typename Stream> void unpack( Stream& s, time_point_sec& );
    template<typename Stream> void pack( Stream& s, const time_point_sec& );
    template<typename Stream> void unpack( Stream& s, std::string& );
    template<typename Stream> void pack( Stream& s, const std::string& );
    template<typename Stream> void unpack( Stream& s, dp::ecc::public_key& );
    template<typename Stream> void pack( Stream& s, const dp::ecc::public_key& );
    template<typename Stream> void unpack( Stream& s, dp::ecc::private_key& );
    template<typename Stream> void pack( Stream& s, const dp::ecc::private_key& );

    template<typename Stream, typename T> inline void pack( Stream& s, const T& v );
    template<typename Stream, typename T> inline void unpack( Stream& s, T& v );

    template<typename Stream, typename T> inline void pack( Stream& s, const std::vector<T>& v );
    template<typename Stream, typename T> inline void unpack( Stream& s, std::vector<T>& v );

    template<typename Stream> inline void pack( Stream& s, const signed_int& v );
    template<typename Stream> inline void unpack( Stream& s, signed_int& vi );

    template<typename Stream> inline void pack( Stream& s, const unsigned_int& v );
    template<typename Stream> inline void unpack( Stream& s, unsigned_int& vi );

    template<typename Stream> inline void pack( Stream& s, const char* v );
    template<typename Stream> inline void pack( Stream& s, const std::vector<char>& value );
    template<typename Stream> inline void unpack( Stream& s, std::vector<char>& value );

    template<typename Stream, typename T, std::size_t N> inline auto pack( Stream& s, const dp::array<T,N>& v) -> std::enable_if_t<is_trivial_array<T>>;
    template<typename Stream, typename T, std::size_t N> inline auto pack( Stream& s, const dp::array<T,N>& v) -> std::enable_if_t<!is_trivial_array<T>>;
    template<typename Stream, typename T, std::size_t N> inline auto unpack( Stream& s, dp::array<T,N>& v) -> std::enable_if_t<is_trivial_array<T>>;
    template<typename Stream, typename T, std::size_t N> inline auto unpack( Stream& s, dp::array<T,N>& v) -> std::enable_if_t<!is_trivial_array<T>>;

    template<typename Stream> inline void pack( Stream& s, const bool& v );
    template<typename Stream> inline void unpack( Stream& s, bool& v );

    template<typename T> inline std::vector<char> pack( const T& v );
    template<typename T> inline T unpack( const std::vector<char>& s );
    template<typename T> inline T unpack( const char* d, uint32_t s );
    template<typename T> inline void unpack( const char* d, uint32_t s, T& v );
} }
