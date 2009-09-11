cmake_minimum_required (VERSION 2.6)
project (bdremote-ng)

SET(CMAKE_INSTALL_PREFIX "")

OPTION(USE_SPLINT "Run splint after compiling" OFF)

#
# Header file checks:
#

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)

CHECK_INCLUDE_FILES ("malloc.h" HAVE_MALLOC_H)
IF(NOT HAVE_MALLOC_H)
  message(FATAL_ERROR "Required malloc header not found.")
ENDIF(NOT HAVE_MALLOC_H)

CHECK_INCLUDE_FILES ("bluetooth/bluetooth.h;bluetooth/hci.h;bluetooth/hci_lib.h;bluetooth/l2cap.h" HAVE_BLUETOOTH_H)
IF(NOT HAVE_BLUETOOTH_H)
  message(FATAL_ERROR "Required bluetooth headers not found.")
ENDIF(NOT HAVE_BLUETOOTH_H)

CHECK_INCLUDE_FILES ("pthread.h" HAVE_PTHREAD_H)
IF(NOT HAVE_PTHREAD_H)
  message(FATAL_ERROR "Required pthread header not found.")
ENDIF(NOT HAVE_PTHREAD_H)

CHECK_INCLUDE_FILES ("getopt.h" HAVE_GETOPT_H)
IF(NOT HAVE_GETOPT_H)
  message(FATAL_ERROR "Required getopt(..) header not found.")
ENDIF(NOT HAVE_GETOPT_H)



#
# Link checks:
# 

CHECK_LIBRARY_EXISTS (bluetooth str2ba "" HAVE_BLUETOOTH_LIB)

IF(NOT HAVE_BLUETOOTH_LIB)
  message(FATAL_ERROR "Required bluetooth library not found.")
ENDIF(NOT HAVE_BLUETOOTH_LIB)

CHECK_LIBRARY_EXISTS (pthread pthread_create "" HAVE_PTHREAD_LIB)

IF(NOT HAVE_PTHREAD_LIB)
  message(FATAL_ERROR "Required pthread library not found.")
ENDIF(NOT HAVE_PTHREAD_LIB)

include(splint.cmake)

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

add_library (bdremoteng_generic ${GENERIC_HEADERS} ${GENERIC_SOURCES} )

IF(USE_SPLINT)

  FOREACH(F ${GENERIC_HEADERS})
    add_splint ("bdremoteng_generic" ${F})  
  ENDFOREACH(F)
  
  FOREACH(F ${GENERIC_SOURCES})
    add_splint ("bdremoteng_generic" ${F})  
  ENDFOREACH(F)

ENDIF(USE_SPLINT)


install(FILES ${GENERIC_SOURCES} ${GENERIC_HEADERS} DESTINATION src)

SET(BLUEZCAP_HEADERS
${PREFIX}/capture/capture_bluez.h
)

SET(BLUEZCAP_SOURCES
${PREFIX}/capture/capture_bluez.c
)

add_library (bdremoteng_bluezcap ${BLUEZCAP_HEADERS} ${BLUEZCAP_SOURCES})

IF(USE_SPLINT)

  FOREACH(F ${BLUEZCAP_HEADERS})
    add_splint ("bdremoteng_bluezcap" ${F})  
  ENDFOREACH(F)
  
  FOREACH(F ${BLUEZCAP_SOURCES})
    add_splint ("bdremoteng_bluezcap" ${F})  
  ENDFOREACH(F)

ENDIF(USE_SPLINT)

install(FILES ${BLUEZCAP_SOURCES} ${BLUEZCAP_HEADERS} DESTINATION src/capture)

#
# Application.
# 

SET(APP_HEADERS
${PREFIX}/bdremoteng.h
${PREFIX}/ug.h
)

SET(APP_SOURCES
${PREFIX}/bdremoteng.c
${PREFIX}/ug.c
)

add_executable (bdremoteng ${APP_HEADERS} ${APP_SOURCES})

IF(USE_SPLINT)

  FOREACH(F ${APP_HEADERS})
    add_splint ("bdremoteng" ${F})  
  ENDFOREACH(F)
  
  FOREACH(F ${APP_SOURCES})
    add_splint ("bdremoteng" ${F})  
  ENDFOREACH(F)

ENDIF(USE_SPLINT)

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

SET(PREFIX "../")

SET(MISC_FILES
${PREFIX}/ChangeLog
${PREFIX}/COPYING
${PREFIX}/AUTHORS
)

install(FILES ${MISC_FILES} DESTINATION .)

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


