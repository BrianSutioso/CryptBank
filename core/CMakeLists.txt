cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# -----------------------------------------------
# -----------------------------------------------
# Setting up libdb

file(GLOB_RECURSE DB_SOURCES src/db/*.cpp)
file(GLOB_RECURSE DB_INCLUDE include/db/*.h)

add_library(db STATIC ${DB_SOURCES} ${DB_INCLUDE})

target_include_directories(db PUBLIC include/db)


# -----------------------------------------------
# -----------------------------------------------
# Setting up libproto

include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIR})

file(GLOB_RECURSE PROTO_SOURCES src/proto/*.cc)
file(GLOB_RECURSE PROTO_HEADERS include/proto/*.h)

add_library(proto STATIC ${PROTO_HEADERS} ${PROTO_SOURCES})
target_include_directories(proto PUBLIC include/proto ${Protobuf_INCLUDE_DIR})
target_link_libraries(proto ${Protobuf_LIBRARIES})


# -----------------------------------------------
# -----------------------------------------------
# Setting up libprimitives

file(GLOB_RECURSE PRIMITIVES_SOURCES src/primitives/*.cpp)
file(GLOB_RECURSE PRIMITIVES_INCLUDE include/primitives/*.h)

add_library(primitives STATIC ${PRIMITIVES_SOURCES} ${PRIMITIVES_INCLUDE})

target_include_directories(primitives PUBLIC include/primitives)
target_link_libraries(primitives proto)


# -----------------------------------------------
# -----------------------------------------------
# Setting up libcrypto

file(GLOB_RECURSE CRYPTO_SOURCES src/crypto/*.cpp)
file(GLOB_RECURSE CRYPTO_INCLUDE include/crypto/*.h)

add_library(crypto STATIC ${CRYPTO_SOURCES} ${CRYPTO_INCLUDE})

target_include_directories(crypto PUBLIC include/crypto)


# -----------------------------------------------
# -----------------------------------------------
# Setting up libchain

file(GLOB_RECURSE CHAIN_SOURCES src/chain/*.cpp)
file(GLOB_RECURSE CHAIN_INCLUDE include/chain/*.h)

file(GLOB_RECURSE CHAIN_WRITER_SOURCES src/chain_writer/*.cpp)
file(GLOB_RECURSE CHAIN_WRITER_INCLUDE include/chain_writer/*.h)

file(GLOB_RECURSE COINDB_SOURCES src/coin_database/*.cpp)
file(GLOB_RECURSE COINDB_INCLUDE include/coin_database/*.h)

file(GLOB_RECURSE BLOCK_INFO_DB_SOURCES src/block_info_database/*.cpp)
file(GLOB_RECURSE BLOCK_INFO_DB_INCLUDE include/block_info_database/*.h)


add_library(CryptBank STATIC ${CHAIN_SOURCES} ${CHAIN_INCLUDE}
        ${CHAIN_WRITER_SOURCES} ${CHAIN_WRITER_INCLUDE}
        ${COINDB_SOURCES} ${COINDB_INCLUDE}
        ${BLOCK_INFO_DB_SOURCES} ${BLOCK_INFO_DB_INCLUDE})


target_include_directories(CryptBank PUBLIC include/chain
        include/chain_writer include/coin_database
        include/block_info_database)
target_link_libraries(CryptBank db primitives crypto)