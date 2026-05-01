include(ECMAddTests)

find_package(Qt6 ${QT_MIN_VERSION} NO_MODULE REQUIRED Test)
set_package_properties(Qt6Test PROPERTIES PURPOSE "Required for autotests")

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

add_test(NAME kcm_cellular_network_smoketest COMMAND kcmshell6 --smoke-test kcm_cellular_network)
set_tests_properties(kcm_cellular_network_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_networkmanagement_smoketest COMMAND kcmshell6 --smoke-test kcm_networkmanagement)
set_tests_properties(kcm_networkmanagement_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_mobile_hotspot_smoketest COMMAND kcmshell6 --smoke-test kcm_mobile_hotspot)
set_tests_properties(kcm_mobile_hotspot_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_mobile_wifi_smoketest COMMAND kcmshell6 --smoke-test kcm_mobile_wifi)
set_tests_properties(kcm_mobile_wifi_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_mobile_wired_smoketest COMMAND kcmshell6 --smoke-test kcm_mobile_wired)
set_tests_properties(kcm_mobile_wired_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)
