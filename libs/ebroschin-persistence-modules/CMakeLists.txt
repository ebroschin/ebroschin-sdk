add_library(ebroschin_persistence_modules
    include/ebroschin/persistence-modules/sqlite/sqlite_persistence_store.hpp
    src/sqlite/sqlite_persistence_store.cpp
)

add_library(ebroschin::persistence-modules ALIAS ebroschin_persistence_modules)
set_target_properties(ebroschin_persistence_modules PROPERTIES EXPORT_NAME persistence-modules)

target_include_directories(ebroschin_persistence_modules PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

find_package(SQLiteCpp CONFIG REQUIRED)

target_link_libraries(ebroschin_persistence_modules PUBLIC
    SQLiteCpp
    ebroschin::utility
    ebroschin::network
    ebroschin::scheduling
    ebroschin::persistence
)

target_compile_features(ebroschin_persistence_modules INTERFACE cxx_std_23)