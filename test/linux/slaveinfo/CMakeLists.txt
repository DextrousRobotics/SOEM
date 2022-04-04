
set(SOURCES slaveinfo.c)

execute_process(COMMAND xeno-config  --posix --cflags OUTPUT_VARIABLE XENO_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND xeno-config  --posix --ldflags OUTPUT_VARIABLE XENO_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${XENO_CFLAGS} ${XENO_LDFLAGS}")
set(CMAKE_C_COMPILER gcc)

include_directories(
  /usr/xenomai/include
  /usr/xenomai/include/cobalt
)

link_directories( /usr/xenomai/lib )


add_executable(slaveinfo ${SOURCES})
target_link_libraries(slaveinfo soem)

install(TARGETS slaveinfo DESTINATION bin)
