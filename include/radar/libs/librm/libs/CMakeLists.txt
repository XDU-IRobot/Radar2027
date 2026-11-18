cmake_minimum_required(VERSION 3.13)

# ETL is used by the device base and referee parser.
unset(BUILD_TESTS) # suppress CMP0077 warning
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/etl)

# Protocol command maps use mapbox::eternal.
add_library(eternal INTERFACE)
target_include_directories(eternal INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/eternal/include
)

# Keep the parent library target compatible with this reduced dependency set.
add_library(third_party INTERFACE)
target_link_libraries(third_party INTERFACE
    etl::etl
    eternal
)
