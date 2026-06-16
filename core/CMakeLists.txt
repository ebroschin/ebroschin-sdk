file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
    src/*.cpp
    include/*.hpp
)

add_library(ebroschin_core
    ${SRC}
        src/simulation_pipeline.cpp
)

add_library(ebroschin::core ALIAS ebroschin_core)
set_target_properties(ebroschin_core PROPERTIES EXPORT_NAME core)

target_include_directories(ebroschin_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(ebroschin_core_tests
        tests/test_system_context.cpp
    )

    target_link_libraries(ebroschin_core_tests PRIVATE
        ebroschin_core
        GTest::gtest
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(ebroschin_core_tests)
endif()

target_compile_features(ebroschin_core PUBLIC cxx_std_23)