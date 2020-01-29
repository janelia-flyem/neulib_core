# find_package(PkgConfig)
# pkg_check_modules(GTEST REQUIRED gtest>=1.7.0)

find_package(GTest REQUIRED)

message(${GTEST_INCLUDE_DIRS})

add_executable(testutilities testutilities.cpp)
target_include_directories(testutilities PRIVATE ${GTEST_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(testutilities neucore ${GTEST_BOTH_LIBRARIES})

add_test(AllTests testutilities)
