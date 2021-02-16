set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

include_directories(${mator_SOURCE_DIR}/src)

find_package(Boost COMPONENTS thread system regex REQUIRED)
find_package(SQLITE3 REQUIRED)

include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${SQLITE3_INCLUDE_DIRS} )

add_library(mator_shared SHARED
            ports.cpp
            ps_tor.cpp
            functions.cpp
            graph.cpp
            node.cpp
            nodeweight.cpp
            preferences.cpp
            pathselection.cpp
            ps_distributor.cpp
            recipientanonymity.cpp
            relationshipanonymity.cpp
            senderanonymity.cpp
            weightedgraph.cpp
            )

target_link_libraries(  mator_shared
                        ${Boost_LIBRARIES}
                        ${SQLITE3_LIBRARIES}
                        )

add_executable(mator main.cpp main_makelargegraph.cpp)
# add_executable(mator_benchmark main_benchmark.cpp)

target_link_libraries(  mator
                        mator_shared)

#target_link_libraries(  mator_benchmark
                        #anoa_shared)

# SA
#execute_process(COMMAND gnuplot plotSA.p)
# RA
#execute_process(COMMAND gnuplot plotRA.p)
# Large
#execute_process(COMMAND gnuplot plotCombined.p)
#Combined
#execute_process(COMMAND gnuplot plotCombined.p)

