cmake_minimum_required (VERSION 2.6)
project (bdremote-ng)

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

SET(BLUEZCAP_HEADERS
${PREFIX}/capture/capture_bluez.h
)

SET(BLUEZCAP_SOURCES
${PREFIX}/capture/capture_bluez.c
)

add_library (bdremoteng_bluezcap ${BLUEZCAP_SOURCES} ${BLUEZCAP_HEADERS})

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

target_link_libraries(capturetest bdremoteng_bluezcap bdremoteng_generic) 
target_link_libraries(capturetest -lbluetooth)
target_link_libraries(capturetest -lpthread)
