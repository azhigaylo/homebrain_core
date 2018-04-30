include(CheckCCompilerFlag)

function(enable_c_compiler_flag_if_supported FLAG)
    string(FIND "${CMAKE_C_FLAGS}" "${FLAG}" FLAG_ALREDY_SET)
    if(FLAG_ALREDY_SET EQUAL -1)
        set(FLAG_TO_TEST "test_${FLAG}_c_flag")
        # Remove '=' characters in variable name
        # to prevent interpreting variable by cmake as 'key=value'
        string(REPLACE "=" "_" FLAG_TO_TEST ${FLAG_TO_TEST})

        # test if not tested yet
        if(NOT DEFINED ${FLAG_TO_TEST})
            check_c_compiler_flag("${FLAG}" ${FLAG_TO_TEST})
            if (${FLAG_TO_TEST})
                set(${FLAG_TO_TEST} ${${FLAG_TO_TEST}} CACHE BOOL "Supported C compiler flag" FORCE)
            else()
                set(${FLAG_TO_TEST} ${${FLAG_TO_TEST}} CACHE BOOL "Unsupported C compiler flag" FORCE)
            endif()
        endif()

        # append to CMAKE_C_FLAGS if supported
        if(${FLAG_TO_TEST})
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}" PARENT_SCOPE)
        endif()
    endif()
endfunction()
