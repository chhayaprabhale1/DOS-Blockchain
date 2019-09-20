/**
 *  @file
 *  @copyright defined in dos/LICENSE
 */
#include <dosiolib/dosio.hpp>

#include "test_api.hpp"

void test_types::types_size() {

   dosio_assert( sizeof(int64_t) == 8, "int64_t size != 8");
   dosio_assert( sizeof(uint64_t) ==  8, "uint64_t size != 8");
   dosio_assert( sizeof(uint32_t) ==  4, "uint32_t size != 4");
   dosio_assert( sizeof(int32_t) ==  4, "int32_t size != 4");
   dosio_assert( sizeof(uint128_t) == 16, "uint128_t size != 16");
   dosio_assert( sizeof(int128_t) == 16, "int128_t size != 16");
   dosio_assert( sizeof(uint8_t) ==  1, "uint8_t size != 1");

   dosio_assert( sizeof(account_name) ==  8, "account_name size !=  8");
   dosio_assert( sizeof(table_name) ==  8, "table_name size !=  8");
   dosio_assert( sizeof(time) ==  4, "time size !=  4");
   dosio_assert( sizeof(dosio::key256) == 32, "key256 size != 32" );
}

void test_types::char_to_symbol() {

   dosio_assert( dosio::char_to_symbol('1') ==  1, "dosio::char_to_symbol('1') !=  1");
   dosio_assert( dosio::char_to_symbol('2') ==  2, "dosio::char_to_symbol('2') !=  2");
   dosio_assert( dosio::char_to_symbol('3') ==  3, "dosio::char_to_symbol('3') !=  3");
   dosio_assert( dosio::char_to_symbol('4') ==  4, "dosio::char_to_symbol('4') !=  4");
   dosio_assert( dosio::char_to_symbol('5') ==  5, "dosio::char_to_symbol('5') !=  5");
   dosio_assert( dosio::char_to_symbol('a') ==  6, "dosio::char_to_symbol('a') !=  6");
   dosio_assert( dosio::char_to_symbol('b') ==  7, "dosio::char_to_symbol('b') !=  7");
   dosio_assert( dosio::char_to_symbol('c') ==  8, "dosio::char_to_symbol('c') !=  8");
   dosio_assert( dosio::char_to_symbol('d') ==  9, "dosio::char_to_symbol('d') !=  9");
   dosio_assert( dosio::char_to_symbol('e') == 10, "dosio::char_to_symbol('e') != 10");
   dosio_assert( dosio::char_to_symbol('f') == 11, "dosio::char_to_symbol('f') != 11");
   dosio_assert( dosio::char_to_symbol('g') == 12, "dosio::char_to_symbol('g') != 12");
   dosio_assert( dosio::char_to_symbol('h') == 13, "dosio::char_to_symbol('h') != 13");
   dosio_assert( dosio::char_to_symbol('i') == 14, "dosio::char_to_symbol('i') != 14");
   dosio_assert( dosio::char_to_symbol('j') == 15, "dosio::char_to_symbol('j') != 15");
   dosio_assert( dosio::char_to_symbol('k') == 16, "dosio::char_to_symbol('k') != 16");
   dosio_assert( dosio::char_to_symbol('l') == 17, "dosio::char_to_symbol('l') != 17");
   dosio_assert( dosio::char_to_symbol('m') == 18, "dosio::char_to_symbol('m') != 18");
   dosio_assert( dosio::char_to_symbol('n') == 19, "dosio::char_to_symbol('n') != 19");
   dosio_assert( dosio::char_to_symbol('o') == 20, "dosio::char_to_symbol('o') != 20");
   dosio_assert( dosio::char_to_symbol('p') == 21, "dosio::char_to_symbol('p') != 21");
   dosio_assert( dosio::char_to_symbol('q') == 22, "dosio::char_to_symbol('q') != 22");
   dosio_assert( dosio::char_to_symbol('r') == 23, "dosio::char_to_symbol('r') != 23");
   dosio_assert( dosio::char_to_symbol('s') == 24, "dosio::char_to_symbol('s') != 24");
   dosio_assert( dosio::char_to_symbol('t') == 25, "dosio::char_to_symbol('t') != 25");
   dosio_assert( dosio::char_to_symbol('u') == 26, "dosio::char_to_symbol('u') != 26");
   dosio_assert( dosio::char_to_symbol('v') == 27, "dosio::char_to_symbol('v') != 27");
   dosio_assert( dosio::char_to_symbol('w') == 28, "dosio::char_to_symbol('w') != 28");
   dosio_assert( dosio::char_to_symbol('x') == 29, "dosio::char_to_symbol('x') != 29");
   dosio_assert( dosio::char_to_symbol('y') == 30, "dosio::char_to_symbol('y') != 30");
   dosio_assert( dosio::char_to_symbol('z') == 31, "dosio::char_to_symbol('z') != 31");

   for(unsigned char i = 0; i<255; i++) {
      if((i >= 'a' && i <= 'z') || (i >= '1' || i <= '5')) continue;
      dosio_assert( dosio::char_to_symbol((char)i) == 0, "dosio::char_to_symbol() != 0");
   }
}

void test_types::string_to_name() {

   dosio_assert( dosio::string_to_name("a") == N(a) , "dosio::string_to_name(a)" );
   dosio_assert( dosio::string_to_name("ba") == N(ba) , "dosio::string_to_name(ba)" );
   dosio_assert( dosio::string_to_name("cba") == N(cba) , "dosio::string_to_name(cba)" );
   dosio_assert( dosio::string_to_name("dcba") == N(dcba) , "dosio::string_to_name(dcba)" );
   dosio_assert( dosio::string_to_name("edcba") == N(edcba) , "dosio::string_to_name(edcba)" );
   dosio_assert( dosio::string_to_name("fedcba") == N(fedcba) , "dosio::string_to_name(fedcba)" );
   dosio_assert( dosio::string_to_name("gfedcba") == N(gfedcba) , "dosio::string_to_name(gfedcba)" );
   dosio_assert( dosio::string_to_name("hgfedcba") == N(hgfedcba) , "dosio::string_to_name(hgfedcba)" );
   dosio_assert( dosio::string_to_name("ihgfedcba") == N(ihgfedcba) , "dosio::string_to_name(ihgfedcba)" );
   dosio_assert( dosio::string_to_name("jihgfedcba") == N(jihgfedcba) , "dosio::string_to_name(jihgfedcba)" );
   dosio_assert( dosio::string_to_name("kjihgfedcba") == N(kjihgfedcba) , "dosio::string_to_name(kjihgfedcba)" );
   dosio_assert( dosio::string_to_name("lkjihgfedcba") == N(lkjihgfedcba) , "dosio::string_to_name(lkjihgfedcba)" );
   dosio_assert( dosio::string_to_name("mlkjihgfedcba") == N(mlkjihgfedcba) , "dosio::string_to_name(mlkjihgfedcba)" );
   dosio_assert( dosio::string_to_name("mlkjihgfedcba1") == N(mlkjihgfedcba2) , "dosio::string_to_name(mlkjihgfedcba2)" );
   dosio_assert( dosio::string_to_name("mlkjihgfedcba55") == N(mlkjihgfedcba14) , "dosio::string_to_name(mlkjihgfedcba14)" );

   dosio_assert( dosio::string_to_name("azAA34") == N(azBB34) , "dosio::string_to_name N(azBB34)" );
   dosio_assert( dosio::string_to_name("AZaz12Bc34") == N(AZaz12Bc34) , "dosio::string_to_name AZaz12Bc34" );
   dosio_assert( dosio::string_to_name("AAAAAAAAAAAAAAA") == dosio::string_to_name("BBBBBBBBBBBBBDDDDDFFFGG") , "dosio::string_to_name BBBBBBBBBBBBBDDDDDFFFGG" );
}

void test_types::name_class() {

   dosio_assert( dosio::name{dosio::string_to_name("azAA34")}.value == N(azAA34), "dosio::name != N(azAA34)" );
   dosio_assert( dosio::name{dosio::string_to_name("AABBCC")}.value == 0, "dosio::name != N(0)" );
   dosio_assert( dosio::name{dosio::string_to_name("AA11")}.value == N(AA11), "dosio::name != N(AA11)" );
   dosio_assert( dosio::name{dosio::string_to_name("11AA")}.value == N(11), "dosio::name != N(11)" );
   dosio_assert( dosio::name{dosio::string_to_name("22BBCCXXAA")}.value == N(22), "dosio::name != N(22)" );
   dosio_assert( dosio::name{dosio::string_to_name("AAAbbcccdd")} == dosio::name{dosio::string_to_name("AAAbbcccdd")}, "dosio::name == dosio::name" );

   uint64_t tmp = dosio::name{dosio::string_to_name("11bbcccdd")};
   dosio_assert(N(11bbcccdd) == tmp, "N(11bbcccdd) == tmp");
}
