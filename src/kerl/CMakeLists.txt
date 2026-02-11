cmake_minimum_required(VERSION "3.30")

set(CSR_GENERATOR ${CMAKE_CURRENT_LIST_DIR}/sys/generate_csrs.py)
set(SVD_FILE      ${CMAKE_CURRENT_LIST_DIR}/sys/rp2040.svd)
set(CSR_OUT_DIR   ${CMAKE_CURRENT_LIST_DIR}/sys/csrs)
set(CSR_STAMP     ${CSR_OUT_DIR}/.csrs.stamp)

add_custom_command(
	OUTPUT  ${CSR_STAMP}
	COMMAND ${Python3_EXECUTABLE} ${CSR_GENERATOR} ${SVD_FILE} -o ${CSR_OUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E touch ${CSR_STAMP}
	DEPENDS ${SVD_FILE} ${CSR_GENERATOR}
	COMMENT "Generating CSR headers from ${SVD_FILE}"
)

add_custom_target(generate_csrs DEPENDS ${CSR_STAMP})

add_library(kerl STATIC
	lib.cc
)

add_dependencies(kerl generate_csrs)

target_include_directories(kerl PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
	${CSR_OUT_DIR}
)

target_compile_features(kerl PUBLIC cxx_std_17)
