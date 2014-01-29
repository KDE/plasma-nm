include_directories(${CMAKE_SOURCE_DIR}/libs
                    ${CMAKE_SOURCE_DIR}/libs/models)

set(plasma_networkmanagement_qml_plugins_SRCS
   availabledevices.cpp
   connectionicon.cpp
   enabledconnections.cpp
   enums.cpp
   globalconfig.cpp
   handler.cpp
   networkstatus.cpp
   trafficmonitor.cpp
   qmlplugins.cpp
)

kde4_add_library(plasmanetworkmanagementplugins SHARED ${plasma_networkmanagement_qml_plugins_SRCS})

target_link_libraries(plasmanetworkmanagementplugins
    plasmanetworkmanagement-internal
    plasmanetworkmanagement-editor
    ${NETWORKMANAGERQT_LDFLAGS}
    ${MODEMMANAGERQT_LDFLAGS}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY})

install(TARGETS plasmanetworkmanagementplugins DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/networkmanagement)
install(FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/networkmanagement)
