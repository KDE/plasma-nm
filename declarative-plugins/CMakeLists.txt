set(plasma_nm_qml_plugins_SRCS
   applet/enabledconnections.cpp
   applet/connectionicon.cpp
   applet/globalstatus.cpp
   applet/trafficmonitor.cpp

   model/model.cpp
   model/modelitem.cpp
#    model/modelwirelessitem.cpp
#    model/modelwireditem.cpp
#    model/modelvpnitem.cpp
#    model/modelmodemitem.cpp
#    model/modelbtitem.cpp
   model/monitor.cpp
   model/sortmodel.cpp

   debug.cpp
   handler.cpp
   qmlplugins.cpp
)

kde4_add_library(plasmanmplugins SHARED ${plasma_nm_qml_plugins_SRCS})

target_link_libraries(plasmanmplugins
    NetworkManagerQt
    ModemManagerQt
    plasmanm-internal
    ${KDE4_PLASMA_LIBS}
    ${QT_QTNETWORK_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY})

install(TARGETS plasmanmplugins DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/plasmanm)
install(FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/plasmanm)
