set(plasma_networkmanagement_qml_plugins_SRCS
   applet/availabledevices.cpp
   applet/connectionicon.cpp
   applet/enabledconnections.cpp
   applet/globalconfig.cpp
   applet/networkstatus.cpp
   applet/trafficmonitor.cpp

   model/networkmodel.cpp
   model/networkmodelitem.cpp
   model/networkitemslist.cpp
   model/networksortmodel.cpp

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
    ${KDE4_PLASMA_LIBS}
    ${QT_QTNETWORK_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY})

install(TARGETS plasmanetworkmanagementplugins DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/networkmanagement)
install(FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/networkmanagement)
