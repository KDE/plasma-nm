include(ECMAddTests)

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Test)
set_package_properties(Qt${QT_MAJOR_VERSION}Test PROPERTIES PURPOSE "Required for autotests")

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

ecm_add_test(
    mobileproviderstest.cpp
    LINK_LIBRARIES Qt::Test plasmanm_editor
)
