file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
    src/*.cpp
    include/*.hpp
)

add_library(ebroschin_network_modules
    ${SRC}
)

add_library(ebroschin::network-modules ALIAS ebroschin_network_modules)
set_target_properties(ebroschin_network_modules PROPERTIES EXPORT_NAME network-modules)

target_include_directories(ebroschin_network_modules PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

find_package(Boost REQUIRED COMPONENTS system)

target_link_libraries(ebroschin_network_modules PUBLIC
    ebroschin::utility
    ebroschin::network
    ebroschin::scheduling
    Boost::system
)

if(WIN32)
    target_link_libraries(ebroschin_network_modules PUBLIC ws2_32 mswsock)
endif()

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(ebroschin_network_modules_tests
        tests/test_json_codec.cpp
    )

    target_link_libraries(ebroschin_network_modules_tests PRIVATE
        ebroschin_network_modules
        ebroschin::network
        ebroschin::utility
        ebroschin::core
        GTest::gtest
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(ebroschin_network_modules_tests)
endif()

target_compile_features(ebroschin_network_modules INTERFACE cxx_std_23)