include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set(plasma_nm_qml_plugins_SRCS
   applet/enabledconnections.cpp
   applet/connectionicon.cpp
   applet/globalstatus.cpp

   model/model.cpp
   model/modelitem.cpp
   model/modelwirelessitem.cpp
   model/modelwireditem.cpp
   model/modelvpnitem.cpp
   model/monitor.cpp
   model/sortmodel.cpp

   debug.cpp
   handler.cpp
   passworddialog.cpp
   secretagent.cpp
   vpnuiplugin.cpp
   qmlplugins.cpp
)

kde4_add_ui_files(plasma_nm_qml_plugins_SRCS
    ui/passworddialog.ui
)

kde4_add_library(plasmanmplugins SHARED ${plasma_nm_qml_plugins_SRCS})

target_link_libraries(plasmanmplugins
    QtNetworkManager
    ${QT_QTNETWORK_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY})

install(TARGETS plasmanmplugins DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/plasmanm)
install(FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/plasmanm)
install(FILES plasma-nm-vpnuiplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
