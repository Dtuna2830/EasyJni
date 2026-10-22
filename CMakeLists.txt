cmake_minimum_required(VERSION 3.14)
project(EasyJni)
option(BUILD_TESTS "Build tests" ON)

find_package(JNI REQUIRED)

add_library(EasyJni INTERFACE)
target_include_directories(EasyJni INTERFACE ${JNI_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(EasyJni INTERFACE ${JNI_LIBRARIES})
target_compile_features(EasyJni INTERFACE cxx_std_20)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()