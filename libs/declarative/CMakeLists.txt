include_directories(${CMAKE_SOURCE_DIR}/libs/models)

set(plasma_networkmanagement_qml_plugins_SRCS
   availabledevices.cpp
   connectionicon.cpp
   enabledconnections.cpp
   enums.cpp
   networkstatus.cpp
#    trafficmonitor.cpp
   qmlplugins.cpp
)

add_library(plasmanetworkmanagementplugins SHARED ${plasma_networkmanagement_qml_plugins_SRCS})

target_link_libraries(plasmanetworkmanagementplugins
    plasmanetworkmanagement-internal
    plasmanetworkmanagement-editor
    Qt5::Quick
    Qt5::DBus
    Qt5::Network
    KF5::NetworkManagerQt
    KF5::ModemManagerQt
    KF5::I18n
    KF5::WindowSystem
)

install(TARGETS plasmanetworkmanagementplugins DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/networkmanagement)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/networkmanagement)
