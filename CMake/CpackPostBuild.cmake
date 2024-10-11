# do nothing

#if (EXISTS ${CPACK_OUTPUT_FILE_PATH})
#    find_program(STRIP_EXE NAMES strip-nondeterminism)
#    if (STRIP_EXE)
#        message("Stripping cpack atrifact...")
#        execute_process(COMMAND ${STRIP_EXE} ${CPACK_OUTPUT_FILE_PATH})
#    endif ()
#endif ()
