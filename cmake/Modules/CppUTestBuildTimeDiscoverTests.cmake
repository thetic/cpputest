function (cpputest_buildtime_discover_tests EXECUTABLE)
  set(DISCOVER_SCRIPT ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../Scripts/CppUTestBuildTimeDiscoverTests.cmake)

  add_custom_command (TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -DTESTS_DETAILED:BOOL=${TESTS_DETAILED} -DEXECUTABLE=$<TARGET_FILE:${EXECUTABLE}> -P ${DISCOVER_SCRIPT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Discovering Tests in ${EXECUTABLE}"
    VERBATIM)
endfunction ()
