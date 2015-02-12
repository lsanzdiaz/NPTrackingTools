# Minimum version of CMake required
cmake_minimum_required (VERSION 3.0.2)
set(CMAKE_MACOSX_RPATH 0)

project(NPTrackingTools C CXX)

# Create option for fake API use
option(USE_NPTrackingTools_FAKE OFF)

# Add a configuration file
if(NOT WIN32)
  set(WIN32SYSTEMFLAG "WIN32" CACHE STRING "")
endif()

configure_file (
  "${PROJECT_SOURCE_DIR}/CMake/Config.h.in"
  "${PROJECT_BINARY_DIR}/CmakeFilesHeader/Config.h"
  FORCE)

if(USE_NPTrackingTools_FAKE)
  include(NPTrackingToolsFakeConfiguration.cmake)

else()
  message("=======================================")
  message(" Using REAL NPTrackingTools API        ")
  message("=======================================")

  message(" Attempt to find NPTrackingTools in the system...")

  # Find TrackingTools API (if exist)
  include("${PROJECT_SOURCE_DIR}/CMake/FindTrackingToolsAPI.cmake")

  if(TrackingToolsAPI_FOUND)
    message(" REAL NPTrackingTools API  FOUND       ")
    message(" Generating target and CMake Configuration File")

    # Add a configuration file
    set(INCLUDE_DIR_TEMPLATE  ${TrackingToolsAPI_INCLUDE_DIRS})
    set(SOURCE_DIR_TEMPLATE   ${TrackingToolsAPI_SOURCE_DIRS})
    set(EXECUTABLES_TEMPLATE  ${TrackingToolsAPI_EXECUTABLES})
    set(LIBRARY_TEMPLATE      ${TrackingToolsAPI_LIBRARIES})
    set(BINARY_DIR_TEMPLATE   ${TrackingToolsAPI_BINARY_DIRS})
    set(SHARED_TEMPLATE       ${TrackingToolsAPI_SHARED})
    set(DEFINITIONS_TEMPLATE  ${TrackingToolsAPI_DEFINITIONS})


    # Create installation and export targets for real NPTrackingTools
    configure_file (
      "${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake.in"
      "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
      @ONLY
      FORCE
      )

    # Installation
    #set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/install) ## Where to install the lib

    # install dll, lib and headers
    install(FILES "${TrackingToolsAPI_LIB_DIR}/${TrackingToolsAPI_LIBRARIES}.lib" DESTINATION lib)
    install(FILES "${TrackingToolsAPI_LIB_DIR}/${TrackingToolsAPI_LIBRARIES}.dll" DESTINATION bin)

    # Setting the public headers of the library
    file(GLOB ${PROJECT_NAME}_SOURCE_HDR_FILES ${TrackingToolsAPI_INCLUDE_DIRS}/*.h)
    file(GLOB ${PROJECT_NAME}_CMAKE_HDR_FILES ${PROJECT_BINARY_DIR}/CmakeFilesHeader/*.h)
    list(APPEND ${PROJECT_NAME}_HDR_FILES ${${PROJECT_NAME}_SOURCE_HDR_FILES} ${${PROJECT_NAME}_CMAKE_HDR_FILES})

    install(FILES ${${PROJECT_NAME}_HDR_FILES} DESTINATION inc)

    # install the configure files
    # Create installation and export targets for real NPTrackingTools
    configure_file (
      "${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}/${PROJECT_NAME}-targets-debug.cmake.in"
      "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-debug.cmake"
      @ONLY
      FORCE
      )
    configure_file (
      "${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}/${PROJECT_NAME}-targets-minsizerel.cmake.in"
      "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-minsizerel.cmake"
      @ONLY
      FORCE
      )
    configure_file (
      "${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}/${PROJECT_NAME}-targets-release.cmake.in"
      "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-release.cmake"
      @ONLY
      FORCE
      )
    configure_file (
      "${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}/${PROJECT_NAME}-targets-relwithdebinfo.cmake.in"
      "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-relwithdebinfo.cmake"
      @ONLY
      FORCE
      )
    configure_file (
      "${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}/${PROJECT_NAME}-targets.cmake.in"
      "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets.cmake"
      @ONLY
      FORCE
      )

  else()
    message("NPTrackingTools NOT FOUND, using fake NPTrackingTools...")
    message("                setting USE_NPTrackingTools_FAKE to ON..")
    set(USE_NPTrackingTools_FAKE ON FORCE)
    include(NPTrackingToolsFakeConfiguration.cmake)
  endif()

endif()

# Documentation
option(BUILD_DOCUMENTATION "" OFF)

if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

    if(BUILD_DOCUMENTATION)
        if(NOT DOXYGEN_FOUND)
            message(FATAL_ERROR "Doxygen is needed to build the documentation.")
        endif()

        set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
        set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        configure_file(${doxyfile_in} ${doxyfile} @ONLY)

        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)

        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION doc/${PROJECT_NAME})
    endif()
endif()


# Python wrapping











