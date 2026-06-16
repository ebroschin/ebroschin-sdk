add_library(ebroschin_logging_modules
    include/ebroschin/logging-modules/spdlog/spdlog-logger.hpp
    src/spdlog/spdlog-logger.cpp
    include/ebroschin/logging-modules/stacktrace/boost_stacktrace.hpp
)

add_library(ebroschin::logging-modules ALIAS ebroschin_logging_modules)
set_target_properties(ebroschin_logging_modules PROPERTIES EXPORT_NAME logging-modules)

target_include_directories(ebroschin_logging_modules PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(WIN32)
    set(BOOST_STACKTRACE stacktrace_windbg)
else()
    set(BOOST_STACKTRACE stacktrace_backtrace)
endif()

find_package(spdlog CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS ${BOOST_STACKTRACE})

target_link_libraries(ebroschin_logging_modules PUBLIC
    Boost::${BOOST_STACKTRACE}
    ebroschin::logging
    spdlog::spdlog
)

target_compile_features(ebroschin_logging_modules PUBLIC cxx_std_23)