cmake_minimum_required(VERSION 3.10)

project(sps30_cli CXX)


include(${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


add_subdirectory(libs)

add_executable(sps30_cli src/sps30_cli.cpp)

target_link_libraries(sps30_cli 
  sps30_uart
  ${CONAN_LIBS}
)

