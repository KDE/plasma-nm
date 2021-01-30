include_directories( ${CMAKE_SOURCE_DIR}/libs/editor )

########### next target ###############

include(ECMAddTests)

find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Test)
set_package_properties(Qt5Test PROPERTIES PURPOSE "Required for autotests")

ecm_add_test(
    simpleipv6test.cpp
    LINK_LIBRARIES Qt::Test plasmanm_editor
)

ecm_add_test(
    simpleipv4test.cpp
    LINK_LIBRARIES Qt::Test plasmanm_editor
)

ecm_add_test(
    simpleiplisttest.cpp
    LINK_LIBRARIES Qt::Test plasmanm_editor
)
