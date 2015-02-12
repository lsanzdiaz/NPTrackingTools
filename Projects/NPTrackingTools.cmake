
set(EP_OPTION_NAME "USE_${EP_NAME}")
set(EP_REQUIRED_PROJECTS)
set(EP_OPTION_DESCRIPTION "NPTrackingTools")
set(EP_OPTION_DEFAULT ON)

cma_list(APPEND EP_REQUIRED_PROJECTS Doxygen IF ${PROJECT_NAME}_BUILD_DOCUMENTATION)

cma_end_definition()
# -----------------------------------------------------------------------------

# Options pop up
cmake_dependent_option(USE_FAKE_NPTrackingTools "Use Fake Optitrack API" ON ${EP_OPTION_NAME} ON)


set(EP_CMAKE_ARGS
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
  -DCMAKE_INSTALL_PREFIX:PATH=${DESTDIR}
  -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
  -DUSE_NPTrackingTools_FAKE:BOOL=${USE_FAKE_NPTrackingTools}
  -DBUILD_DOCUMENTATION:BOOL=OFF
  )

if(${${PROJECT_NAME}_BUILD_DOCUMENTATION})
  list(APPEND EP_CMAKE_ARGS
    -DBUILD_DOCUMENTATION:BOOL=ON
    -DDOXYGEN_EXECUTABLE:PATH=${${PROJECT_NAME}_DOXYGEN_EXECUTABLE}
    )
endif()

ExternalProject_Add(${EP_NAME}
  DEPENDS ${EP_REQUIRED_PROJECTS}
  # download
  # patch
  # update
  UPDATE_COMMAND ""
  # configure
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/${EP_NAME}
  CMAKE_ARGS ${EP_CMAKE_ARGS}
  # build
  BINARY_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}-build
  # install
  INSTALL_DIR ${DESTDIR}
  # test
  )

set(${PROJECT_NAME}_${EP_NAME}_DIR "${PROJECT_BINARY_DIR}/${EP_NAME}-build" CACHE INTERNAL "")