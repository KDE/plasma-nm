set(plasma_networkmanagement_qml_plugins_SRCS
   applet/availabledevices.cpp
   applet/connectionicon.cpp
   applet/enabledconnections.cpp
   applet/globalconfig.cpp
   applet/networkstatus.cpp
   #applet/trafficmonitor.cpp

   model/model.cpp
   model/modelitem.cpp
   model/modelitems.cpp
   model/monitor.cpp
   model/sortmodel.cpp

   debug.cpp
   enums.cpp
   handler.cpp
   qmlplugins.cpp
)

kde4_add_library(plasmanetworkmanagementplugins SHARED ${plasma_networkmanagement_qml_plugins_SRCS})

target_link_libraries(plasmanetworkmanagementplugins
    ${NETWORKMANAGERQT_LDFLAGS}
    ${MODEMMANAGERQT_LDFLAGS}
    ${MODEMMANAGERQT_LIBRARY}
    plasmanetworkmanagement-internal
    plasmanetworkmanagement-editor
    Qt5::Quick
    Qt5::DBus
    Qt5::Network
    KF5::KI18n
    KF5::KWindowSystem
)

install(TARGETS plasmanetworkmanagementplugins DESTINATION ${QML_INSTALL_DIR}/org/kde/networkmanagement)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/networkmanagement)
