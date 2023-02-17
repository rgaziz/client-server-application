set(target server)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

file(GLOB server_sources
    ${PROJECT_SOURCE_DIR}/src/common/settings.h
    ${PROJECT_SOURCE_DIR}/src/common/logger.h
    ${PROJECT_SOURCE_DIR}/src/common/file.cc
    ${PROJECT_SOURCE_DIR}/src/common/file.h

    ${CMAKE_CURRENT_SOURCE_DIR}/server.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/server.h
    ${CMAKE_CURRENT_SOURCE_DIR}/queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cc)

add_executable(${target} ${server_sources})

target_include_directories(${target}
     PRIVATE
       ${PROJECT_SOURCE_DIR}/src)

set_property(TARGET ${target}
     PROPERTY
       CXX_STANDARD 17)
