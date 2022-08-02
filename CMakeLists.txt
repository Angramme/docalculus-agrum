project(DOCALC)
cmake_minimum_required(VERSION "3.0")
# classical options 
# -DCMAKE_BUILD_TYPE=Debug|Release 
# -G "MinGW Makefiles" 

# cmake -DCMAKE_BUILD_TYPE=Debug
# or
# cmake -DCMAKE_BUILD_TYPE=Release
#     Release is the default option (thanks to the next lines)

set (CMAKE_CXX_STANDARD 14)

find_package(OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include("${DOCALC_SOURCE_DIR}/config.txt")
if (NOT DEFINED aGrUM_DIR)
  set(aGrUM_DIR "${AGRUM_INSTALLATION_DIRECTORY}/lib/cmake/aGrUM/")
endif ()

find_package(aGrUM)

if (aGrUM_FOUND)
  include(${AGRUM_USE_FILE})
else (aGrUM_FOUND)
  message(FATAL_ERROR "Please install aGrUM")
endif (aGrUM_FOUND)

file(GLOB DOCALC_SOURCE ${DOCALC_SOURCE_DIR}/src/*.cpp)
file(GLOB DOCALC_INCLUDE ${DOCALC_SOURCE_DIR}/src/*.h)

add_executable (main ${DOCALC_SOURCE})
target_link_libraries(main agrum)

install (TARGETS main DESTINATION bin)

