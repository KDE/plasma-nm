set(plasma_nm_qml_plugins_SRCS
   applet/availabledevices.cpp
   applet/connectionicon.cpp
   applet/enabledconnections.cpp
   applet/globalconfig.cpp
   applet/networkstatus.cpp
   applet/trafficmonitor.cpp

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

kde4_add_library(plasmanmplugins SHARED ${plasma_nm_qml_plugins_SRCS})

target_link_libraries(plasmanmplugins
    NetworkManagerQt
    ModemManagerQt
    plasmanm-internal
    plasmanm-editor
    ${KDE4_PLASMA_LIBS}
    ${QT_QTNETWORK_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY})

install(TARGETS plasmanmplugins DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/plasmanm)
install(FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/plasmanm)
