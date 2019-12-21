cmake_minimum_required(VERSION 3.5)

project(neucore)

add_library(neucore SHARED src/utilities.cpp)
target_include_directories(neucore PUBLIC ${PROJECT_SOURCE_DIR}/include)

install(TARGETS neucore LIBRARY DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include/neulib)
