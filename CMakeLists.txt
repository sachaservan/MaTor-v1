cmake_minimum_required(VERSION 2.8)

project(mator_project)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CXXFLAGS -O2)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(OperatingSystem "Linux")
    add_custom_target(largegraph)
    add_custom_target(graph)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(OperatingSystem "MacOSX")
    add_custom_target(largegraph)
    add_custom_target(graph)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

set(RELEASE_FOLDER Release)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/lib)

configure_file(src/config.cfg "${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/bin/config.cfg" COPYONLY)

configure_file(src/construct_database.py  ${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/bin/construct_database.py COPYONLY)
configure_file(src/download_consensus.py  ${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/bin/download_consensus.py COPYONLY)
configure_file(src/longtime_analysis.py  ${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/bin/longtime_analysis.py COPYONLY)
file(COPY src/stem DESTINATION ${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/bin/)

configure_file(data/2014-02-05-03-00-00-consensus ${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/data/2014-02-05-03-00-00-consensus COPYONLY)

#configure_file(src/plotCombined.p ${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/data/plotCombined.p COPYONLY)
#configure_file(src/plotLarge.p  ${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/data/plotLarge.p COPYONLY)
#configure_file(src/plotRA.p  ${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/data/plotRA.p COPYONLY)
#configure_file(src/plotSA.p  ${CMAKE_BINARY_DIR}/${RELEASE_FOLDER}/data/plotSA.p COPYONLY)

add_subdirectory (src)
