# CMakeLists.txt KEBI project.
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(KEBI)



#---Options
option(KEBI_BUILD_GEANT4 "Build Geant4 simulation" on)
option(CREATE_GIT_LOG "Create git-log at when building KEBI" on)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" on)



#---Add source directories. Directory should be under "/path/to/KEBI/"
list(APPEND KEBI_SOURCE_DIRECTORY_LIST
  source/fit
  source/tool
  source/base
  source/format
  source/tracklet
  source/detector
  source/reco/PSA
  source/analysis/PlotChannels

  GETDecoder/source/GETCore
  GETDecoder/source/GETFormat
  GETDecoder/source/GETUtil

  LAMPS/detector
  LAMPS/task
)

#---Add geant4 source directories. Directory should be under "/path/to/KEBI/".
list(APPEND KEBI_GEANT4_DIRECTORY_LIST
  source/geant4
  LAMPS/geant4
)

#---Add geant4 main programs. Directory should be under "/path/to/KEBI/"
list(APPEND KEBI_GEANT4_EXECUTABLE_LIST
  LAMPS/macros/mc_tpc.cc
)



#=====================================================================================
# BELOW IS NOT TO BE TOUCHED BY USERS GENERALLY
#=====================================================================================



# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
#---libs: Core RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread MultiProc pthread
#---evelibs: Eve EG TreePlayer Geom Ged RGL Gui Core RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread MultiProc pthread
find_package(ROOT REQUIRED COMPONENTS RIO Net Eve Geom Gui Graf Graf3d Ged RGL Matrix MathCore)

include(${ROOT_USE_FILE})

if(KEBI_BUILD_GEANT4)
  find_package(Geant4 REQUIRED ui_all vis_all)
  include(${Geant4_USE_FILE})
endif(KEBI_BUILD_GEANT4)

foreach(_directory ${KEBI_SOURCE_DIRECTORY_LIST})
  include_directories(${CMAKE_SOURCE_DIR}/${_directory})
  if(KEBI_BUILD_GEANT4)
    foreach(_g4directory ${KEBI_GEANT4_DIRECTORY_LIST})
      include_directories(${CMAKE_SOURCE_DIR}/${_g4directory})
    endforeach(_g4directory)
  endif(KEBI_BUILD_GEANT4)

  string(REPLACE / _ _name ${_directory})
  list(APPEND KEBI_SOURCE_NAME_LIST ${_name})
  set(KEBI_${_name}_G_NAME G__${_name}Dict)
  file(GLOB KEBI_${_name}_LINKDEF_FILE ${CMAKE_SOURCE_DIR}/${_directory}/LinkDef.h)
  file(GLOB KEBI_${_name}_SOURCE_FILES ${CMAKE_SOURCE_DIR}/${_directory}/*cc)
  file(GLOB KEBI_${_name}_HEADER_FILES ${CMAKE_SOURCE_DIR}/${_directory}/*hh)

  #message(STATUS "[" ${_name} "]")
  #message(STATUS "  LinkDef file: " ${KEBI_${_name}_LINKDEF_FILE})
  foreach(_file ${KEBI_${_name}_SOURCE_FILES})
    #message(STATUS "  source  file: " ${_file})
  endforeach(_file)
endforeach(_directory)

include_directories(${CMAKE_SOURCE_DIR}/log)

add_definitions(${ROOT_CXX_FLAGS})

foreach(_name ${KEBI_SOURCE_NAME_LIST})
  ROOT_GENERATE_DICTIONARY(${KEBI_${_name}_G_NAME}
    ${KEBI_${_name}_HEADER_FILES}
    LINKDEF ${KEBI_${_name}_LINKDEF_FILE}
  )
endforeach(_name)

foreach(_name ${KEBI_SOURCE_NAME_LIST})
  foreach(_file ${KEBI_${_name}_SOURCE_FILES})
    list(APPEND KEBI_LIB_LIST ${_file})
  endforeach(_file)
  list(APPEND KEBI_LIB_LIST ${KEBI_${_name}_G_NAME}.cxx)
endforeach(_name)

add_library(KEBI SHARED ${KEBI_LIB_LIST})
target_link_libraries(KEBI ${ROOT_LIBRARIES})

if(KEBI_BUILD_GEANT4)
  foreach(_directory ${KEBI_GEANT4_DIRECTORY_LIST})
    set(KEBI_GEANT4_SOURCE_FILES_TEMP)
    file(GLOB KEBI_GEANT4_SOURCE_FILES_TEMP ${CMAKE_SOURCE_DIR}/${_directory}/*.cc)
    foreach(_file ${KEBI_GEANT4_SOURCE_FILES_TEMP})
      list(APPEND KEBI_GEANT4_SOURCE_FILES ${_file})
    endforeach(_file)

    set(KEBI_GEANT4_HEADER_FILES_TEMP)
    file(GLOB KEBI_GEANT4_HEADER_FILES_TEMP ${CMAKE_SOURCE_DIR}/${_directory}/*.hh)
    foreach(_file ${KEBI_GEANT4_HEADER_FILES_TEMP})
      list(APPEND KEBI_GEANT4_HEADER_FILES ${_file})
    endforeach(_file)
  endforeach(_directory)

  #message(STATUS "[Geant4 sources]")
  foreach(_file ${KEBI_GEANT4_SOURCE_FILES})
    #message(STATUS "  source file: " ${_file})
  endforeach(_file)

  foreach(_g4sim ${KEBI_GEANT4_EXECUTABLE_LIST})
    get_filename_component(_g4sim_name ${CMAKE_SOURCE_DIR}/${_g4sim} NAME)
    get_filename_component(_g4sim_directory ${CMAKE_SOURCE_DIR}/${_g4sim} DIRECTORY)
    string(REPLACE .cc "" _executable_head ${_g4sim_name})
    string(REPLACE .cc .g4sim _executable_name ${_g4sim_name})
    #message(STATUS "[Geat4 simulation " ${_executable_head} "]")
    #message(STATUS "  executable main: " ${_g4sim_directory}/${_g4sim_name})
    #message(STATUS "  executable file: " ${_g4sim_directory}/${_executable_name})

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${_g4sim_directory})
    add_executable(${_executable_name} ${CMAKE_SOURCE_DIR}/${_g4sim} ${KEBI_GEANT4_SOURCE_FILES} ${KEBI_GEANT4_HEADER_FILES})
    EXECUTE_PROCESS(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(CMAKE_EXE_LINKER_FLAGS ${ROOT_LD_FLAGS})
    target_link_libraries(${_executable_name} ${Geant4_LIBRARIES} KEBI)
  endforeach(_g4sim)
endif(KEBI_BUILD_GEANT4)

set(KEBI_PATH ${CMAKE_SOURCE_DIR})
configure_file(${KEBI_PATH}/log/KBLog.cmake.in ${KEBI_PATH}/log/KBLog.cmake @ONLY)
add_custom_target(KEBI_LOG ALL
                  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/log/KBLog.cmake
                  VERBATIM)

add_dependencies(KEBI KEBI_LOG)

file(MAKE_DIRECTORY ${KEBI_PATH}/data)
