
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS system thread REQUIRED) 

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS}) 
  add_executable(manager manager.cpp main.cpp)
  target_link_libraries(manager soem ${Boost_LIBRARIES})
  install(TARGETS manager DESTINATION bin)
endif()

