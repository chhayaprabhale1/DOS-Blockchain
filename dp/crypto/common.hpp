#pragma once
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/ripemd160.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/reflect/reflect.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/crypto/base58.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/io/raw.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/utility.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/static_variant.hpp>

namespace dp { namespace crypto {
   template<typename DataType>
   struct checksummed_data {
      checksummed_data() {}
      uint32_t     check = 0;
      DataType     data;

      static auto calculate_checksum(const DataType& data, const char *prefix = nullptr) {
         auto encoder = ripemd160::encoder();
         encoder.write((const char *)&data, sizeof(DataType));

         if (prefix != nullptr) {
            encoder.write(prefix, const_strlen(prefix));
         }
         return encoder.result()._hash[0];
      }
   };

   inline bool prefix_matches(const char* prefix, const std::string& str) {
      auto prefix_len = const_strlen(prefix);
      return str.size() > prefix_len && str.substr(0, prefix_len).compare(prefix) == 0;
   }

   template<typename, const char * const *, int, typename ...>
   struct base58_str_parser_impl;

   template<typename Result, const char * const * Prefixes, int Position, typename KeyType, typename ...Rem>
   struct base58_str_parser_impl<Result, Prefixes, Position, KeyType, Rem...> {
      static Result apply(const std::string& prefix_str, const std::string& data_str)
      {
         using data_type = typename KeyType::data_type;
         using wrapper = checksummed_data<data_type>;
         constexpr auto prefix = Prefixes[Position];

         if (prefix == prefix_str) {
            auto bin = dp::from_base58(data_str);
            DP_ASSERT(bin.size() >= sizeof(data_type) + sizeof(uint32_t));
            auto wrapped = dp::raw::unpack<wrapper>(bin);
            auto checksum = wrapper::calculate_checksum(wrapped.data, prefix);
            DP_ASSERT(checksum == wrapped.check);
            return Result(KeyType(wrapped.data));
         }

         return base58_str_parser_impl<Result, Prefixes, Position + 1, Rem...>::apply(prefix_str, data_str);
      }
   };

   template<typename Result, const char * const * Prefixes, int Position>
   struct base58_str_parser_impl<Result, Prefixes, Position> {
      static Result apply(const std::string& prefix_str, const std::string& data_str ) {
         DP_ASSERT(false, "No matching suite type for ${prefix}_${data}", ("prefix", prefix_str)("data",data_str));
      }
   };

   template<typename, const char * const * Prefixes>
   struct base58_str_parser;

   /**
    * Destructure a variant and call the parse_base58str on it
    * @tparam Ts
    * @param base58str
    * @return
    */
   template<const char * const * Prefixes, typename ...Ts>
   struct base58_str_parser<dp::static_variant<Ts...>, Prefixes> {
      static dp::static_variant<Ts...> apply(const std::string& base58str) {
         const auto pivot = base58str.find('_');
         DP_ASSERT(pivot != std::string::npos, "No delimiter in data, cannot determine suite type: ${str}", ("str", base58str));

         const auto prefix_str = base58str.substr(0, pivot);
         auto data_str = base58str.substr(pivot + 1);
         DP_ASSERT(!data_str.empty(), "Data only has suite type prefix: ${str}", ("str", base58str));

         return base58_str_parser_impl<dp::static_variant<Ts...>, Prefixes, 0, Ts...>::apply(prefix_str, data_str);
      }
   };

   template<typename Storage, const char * const * Prefixes, int DefaultPosition = -1>
   struct base58str_visitor : public dp::visitor<std::string> {
      template< typename KeyType >
      std::string operator()( const KeyType& key ) const {
         using data_type = typename KeyType::data_type;
         constexpr int position = Storage::template position<KeyType>();
         constexpr bool is_default = position == DefaultPosition;

         checksummed_data<data_type> wrapper;
         wrapper.data = key.serialize();
         wrapper.check = checksummed_data<data_type>::calculate_checksum(wrapper.data, !is_default ? Prefixes[position] : nullptr);
         auto packed = raw::pack( wrapper );
         auto data_str = to_base58( packed.data(), packed.size() );
         if (!is_default) {
            data_str = string(Prefixes[position]) + "_" + data_str;
         }

         return data_str;
      }
   };

   template<typename Storage>
   struct eq_comparator {
      struct visitor : public dp::visitor<bool> {
         visitor(const Storage &b)
            : _b(b) {}

         template<typename KeyType>
         bool operator()(const KeyType &a) const {
            const auto &b = _b.template get<KeyType>();
            return a.serialize() == b.serialize();
         }

         const Storage &_b;
      };

      static bool apply(const Storage& a, const Storage& b) {
         return a.which() == b.which() && a.visit(visitor(b));
      }
   };

   template<typename Storage>
   struct less_comparator {
      struct visitor : public dp::visitor<bool> {
         visitor(const Storage &b)
            : _b(b) {}

         template<typename KeyType>
         bool operator()(const KeyType &a) const {
            const auto &b = _b.template get<KeyType>();
            return a.serialize() < b.serialize();
         }

         const Storage &_b;
      };

      static bool apply(const Storage& a, const Storage& b) {
         return a.which() < b.which() || (a.which() == b.which() && a.visit(visitor(b)));
      }
   };

   template<typename Data>
   struct shim {
      using data_type = Data;

      shim()
      {}

      shim(data_type&& data)
      :_data(forward<data_type>(data))
      {}

      shim(const data_type& data)
      :_data(data)
      {}

      const data_type& serialize() const {
         return _data;
      }

      data_type _data;
   };

} }

DP_REFLECT_TEMPLATE((typename T), dp::crypto::checksummed_data<T>, (data)(check) )
DP_REFLECT_TEMPLATE((typename T), dp::crypto::shim<T>, (_data) )
