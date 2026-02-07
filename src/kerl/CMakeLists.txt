cmake_minimum_required(VERSION "3.30")

add_library(kerl STATIC
	lib.cc
)

target_include_directories(kerl PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
)

target_compile_features(kerl PUBLIC cxx_std_17)
