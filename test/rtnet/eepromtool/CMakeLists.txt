
set(SOURCES eepromtool.c)

if (USE_XENOMAI)
  execute_process(COMMAND xeno-config  --posix --cflags OUTPUT_VARIABLE XENO_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND xeno-config  --posix --ldflags OUTPUT_VARIABLE XENO_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${XENO_CFLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${XENO_LDFLAGS}")
endif()

add_executable(eepromtool ${SOURCES})
target_link_libraries(eepromtool soem)
install(TARGETS eepromtool DESTINATION bin)
