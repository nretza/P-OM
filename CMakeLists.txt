#----------------------------------------------------------------------------
# Setup the project

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(optical_module)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable

option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${Geant4_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Setup ROOT include directories and compile definitions
# Setup include directory for this project

# TODO: use ROOT for file output

#find_package(ROOT CONFIG REQUIRED)
#include(${ROOT_USE_FILE})
#include_directories(${ROOT_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Setup Boost include directories and compile definitions
# Setup include directory for this project

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


add_executable(optical_module main.cpp ${sources} ${headers})
target_link_libraries(optical_module ${Geant4_LIBRARIES})


#----------------------------------------------------------------------------
# copy runtime macros in build/
#

set(MACROS
  init.mac
  init_physics.mac
  init_primary_photon.mac
  init_primary_mu.mac
  init_geom.mac
  init_vis.mac
  init_data.mac
  run.mac
  optical_properties.cfg
  )

foreach(_script ${MACROS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/macros/${_script}
    ${PROJECT_BINARY_DIR}/macros/${_script}
    COPYONLY
    )
endforeach()

set(GEOM
  Hamamatsu_R14374_tube_with_tail.gdml
  P-OM_module_v11
  P-OM_module_v13
  P-OM_hemisphere_v11
  P-OM_hemisphere_v13)

foreach(_geom ${GEOM})
  file( COPY        ${PROJECT_SOURCE_DIR}/geometry/${_geom}
        DESTINATION ${PROJECT_BINARY_DIR}/geometry/)
endforeach()