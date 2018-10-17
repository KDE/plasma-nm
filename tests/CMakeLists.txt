set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

########### next target ###############

include(ECMAddTests)

ecm_add_test(
    simpleipv6test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/editor/simpleipv6addressvalidator.cpp
    TEST_NAME simpleipv6test
    LINK_LIBRARIES Qt5::Test  Qt5::Widgets
)

ecm_add_test(
    simpleipv4test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/editor/simpleipv4addressvalidator.cpp
    TEST_NAME simpleipv4test
    LINK_LIBRARIES Qt5::Test  Qt5::Widgets
)

ecm_add_test(
    simpleiplisttest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/editor/simpleipv4addressvalidator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/editor/simpleipv6addressvalidator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/editor/simpleiplistvalidator.cpp
    TEST_NAME simpleiplisttest
    LINK_LIBRARIES Qt5::Test  Qt5::Widgets
)
