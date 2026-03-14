add_test(INTERPOLATOR InterpTest.x)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/dueca
  ${CMAKE_SOURCE_DIR}/dueca)


#execute_process(COMMAND
#  bash ${CMAKE_BINARY_DIR}/scripts/dueca-config --docbuildlibs --nomain
#  OUTPUT_VARIABLE DUECA_LDFLAGS
#  OUTPUT_STRIP_TRAILING_WHITESPACE)

add_executable(InterpTest.x InterpTest.cxx)
target_link_libraries(InterpTest.x dueca-extra${STATICSUFFIX})
