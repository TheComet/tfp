###############################################################################
# source files and library definition
###############################################################################

add_executable (tfp_application
    "src/main.cpp")
target_compile_options (tfp_application
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-W -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-unused-local-typedefs -Wno-long-long -Wno-variadic-macros -Wno-missing-field-initializers -Wno-write-strings>
        $<$<CXX_COMPILER_ID:Clang>:-W -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-unused-local-typedefs -Wno-long-long -Wno-variadic-macros -Wno-missing-field-initializers -Wno-write-strings>)
target_link_libraries (tfp_application
    PRIVATE
        tfp)

###############################################################################
# plugins
###############################################################################

#add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/bode-plot")
#add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/butterworth-filter")
#add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/chebyshev-filter")
#add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/complex-plane-3d")
#add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/dpsfg")
#add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/elliptic-filter")
#add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/pole-zero-plot")
#add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/std-low-order-filters")
#add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/step-impulse-response")
