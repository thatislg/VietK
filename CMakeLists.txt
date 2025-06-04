cmake_minimum_required(VERSION 3.20)
project(VietK CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Thêm flag thử nghiệm nếu cần
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-std=c++23)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/std:c++latest)
endif()

# Chỉ định đường dẫn Windows SDK
set(WIN_SDK_VERSION "10.0.22621.0") # Thay bằng phiên bản SDK của bạn
set(WIN_SDK_PATH "C:/Program Files (x86)/Windows Kits/10")

add_library(VietK SHARED
        src/VietKEngine.cpp
        src/VietKEngine.h
        src/TSFIntegration.cpp
        src/TSFIntegration.h
        src/dllmain.cpp
)

target_link_libraries(VietK PRIVATE ole32 uuid)

target_compile_definitions(VietK PRIVATE UNICODE _UNICODE)

set_target_properties(VietK PROPERTIES
        OUTPUT_NAME "VietK"
        PREFIX ""
        LINK_FLAGS "-Wl,--output-def,VietK.def"
)