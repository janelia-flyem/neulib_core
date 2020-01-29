cmake_minimum_required(VERSION 3.5)

project(neucore)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(neucore SHARED src/utilities.cpp)
target_include_directories(neucore PUBLIC ${PROJECT_SOURCE_DIR}/include)

install(TARGETS neucore LIBRARY DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include/neulib)

option(PACKAGE_TESTS "Build the tests" ON)
if (PACKAGE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
