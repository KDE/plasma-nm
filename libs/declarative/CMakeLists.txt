include_directories(models)

set(plasmanm_qml_plugins_SRCS
    # FIXME
    ../debug.cpp

    availabledevices.cpp
    connectionicon.cpp
    enabledconnections.cpp
    enums.cpp
    networkstatus.cpp
    utils.cpp
    qmlplugins.cpp

    models/appletproxymodel.cpp
    models/creatableconnectionsmodel.cpp
    models/editorproxymodel.cpp
    models/kcmidentitymodel.cpp
    models/networkitemslist.cpp
    models/networkmodel.cpp
    models/networkmodelitem.cpp
)

add_library(plasmanm_qmlplugins SHARED ${plasmanm_qml_plugins_SRCS})

target_link_libraries(plasmanm_qmlplugins
    plasmanm_internal
    Qt5::Quick
    KF5::NetworkManagerQt
    KF5::I18n
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(plasmanm_qmlplugins KF5::ModemManagerQt)
endif()

install(TARGETS plasmanm_qmlplugins DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/networkmanagement)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/networkmanagement)
