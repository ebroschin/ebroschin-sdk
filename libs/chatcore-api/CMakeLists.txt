add_library(chatcore_api INTERFACE
    include/ebroschin/chat/api.hpp
)

add_library(ebroschin::chatcore-api ALIAS chatcore_api)
set_target_properties(chatcore_api PROPERTIES EXPORT_NAME chatcore-api)

target_include_directories(chatcore_api INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(chatcore_api INTERFACE cxx_std_23)