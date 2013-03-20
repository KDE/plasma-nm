include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set(plasma_nm_qml_plugins_SRCS
   appletinfo.cpp
   debug.cpp
   handler.cpp
   model.cpp
   modelitem.cpp
   monitor.cpp
   sortmodel.cpp
   secretagent.cpp
   qmlplugins.cpp
)

kde4_add_library(plasmanmplugins SHARED ${plasma_nm_qml_plugins_SRCS})

target_link_libraries(plasmanmplugins
    QtNetworkManager
    ${QT_QTNETWORK_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY})

install(TARGETS plasmanmplugins DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/plasmanm)
install(FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/plasmanm)
