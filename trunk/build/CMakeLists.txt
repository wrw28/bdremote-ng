cmake_minimum_required (VERSION 2.6)
project (bdremote-ng)

SET(CMAKE_INSTALL_PREFIX "")

# Prefix used for all files.
SET(PREFIX "../src/")

SET(GENERIC_HEADERS
${PREFIX}/bdrcfg.h  

${PREFIX}/captureif.h
${PREFIX}/globaldefs.h
${PREFIX}/keydef.h
${PREFIX}/lirc_srv.h
)

SET(GENERIC_SOURCES
${PREFIX}/bdrcfg.c
${PREFIX}/captureif.c
${PREFIX}/lirc_callback.c
${PREFIX}/lirc_srv.c
)

include_directories (${PREFIX})

add_library (bdremoteng_generic ${GENERIC_SOURCES} ${GENERIC_HEADERS})

install(FILES ${GENERIC_SOURCES} ${GENERIC_HEADERS} DESTINATION src)

SET(BLUEZCAP_HEADERS
${PREFIX}/capture/capture_bluez.h
)

SET(BLUEZCAP_SOURCES
${PREFIX}/capture/capture_bluez.c
)

add_library (bdremoteng_bluezcap ${BLUEZCAP_SOURCES} ${BLUEZCAP_HEADERS})

install(FILES ${BLUEZCAP_SOURCES} ${BLUEZCAP_HEADERS} DESTINATION src/capture)

#
# Application.
# 

SET(APP_HEADERS
${PREFIX}/bdremoteng.h
)

SET(APP_SOURCES
${PREFIX}/bdremoteng.c
)

add_executable (bdremoteng ${APP_HEADERS} ${APP_SOURCES})

install(FILES ${APP_HEADERS} ${APP_SOURCES} DESTINATION src)

target_link_libraries(bdremoteng bdremoteng_bluezcap bdremoteng_generic) 
target_link_libraries(bdremoteng -lbluetooth)
target_link_libraries(bdremoteng -lpthread)

#
# Capture test.
# 

SET(PREFIX "../test")

SET(TEST_HEADERS
)

SET(TEST_SOURCES
${PREFIX}/capture_test.c
)

add_executable (capturetest ${TEST_HEADERS} ${TEST_SOURCES})

install(FILES ${TEST_HEADERS} ${TEST_SOURCES} DESTINATION test)

target_link_libraries(capturetest bdremoteng_bluezcap bdremoteng_generic) 
target_link_libraries(capturetest -lbluetooth)
target_link_libraries(capturetest -lpthread)

# Extra build files.

SET(PREFIX ".")

SET(BUILD_FILES
${PREFIX}/CMakeLists.txt
${PREFIX}/gen.sh
)

install(FILES ${BUILD_FILES} DESTINATION build)

SET(PREFIX "../doc")

SET(DOCS_FILES
${PREFIX}/keys.txt
${PREFIX}/README
${PREFIX}/TODO
)

install(FILES ${DOCS_FILES} DESTINATION doc)

# 
# Stuff needed to create a source installation.
#

# This does not seem to work, and CPack is very badly documented.'
# And it does not get any better with time.

set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES
  "/build/linux;/.svn/;/.bzr/;~$;${CPACK_SOURCE_IGNORE_FILES}")

SET(CPACK_PACKAGE_EXECUTABLES "bdremoteng;bdremoteng")

include(CPack)
