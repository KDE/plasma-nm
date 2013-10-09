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
    plasmanetworkmanagement-internal
    plasmanetworkmanagement-editor
    KF5::plasma
    KF5::KI18n
    KF5::KWindowSystem
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY})

install(TARGETS plasmanetworkmanagementplugins DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/networkmanagement)
install(FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/networkmanagement)
