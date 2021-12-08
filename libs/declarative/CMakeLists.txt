
add_library(plasmanm_qmlplugins SHARED)
target_sources(plasmanm_qmlplugins PRIVATE
   availabledevices.cpp
   connectionicon.cpp
   enabledconnections.cpp
   enums.cpp
   networkstatus.cpp
   qmlplugins.cpp
)

target_link_libraries(plasmanm_qmlplugins
    plasmanm_internal
    plasmanm_editor
    Qt::Quick
    KF5::I18n
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(plasmanm_qmlplugins KF5::ModemManagerQt)
endif()

install(TARGETS plasmanm_qmlplugins DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/networkmanagement)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/networkmanagement)
