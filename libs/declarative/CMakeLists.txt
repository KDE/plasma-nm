
add_library(plasmanm_qmlplugins SHARED)
target_sources(plasmanm_qmlplugins PRIVATE
   availabledevices.cpp
   connectionicon.cpp
   enabledconnections.cpp
   enums.cpp
   networkstatus.cpp
   qmlplugins.cpp
   availabledevices.h
   connectionicon.h
   enabledconnections.h
   enums.h
   networkstatus.h
   qmlplugins.h
)

target_link_libraries(plasmanm_qmlplugins
    plasmanm_internal
    plasmanm_editor
    Qt::Quick
    KF6::I18n
    KF6::ModemManagerQt
)

install(TARGETS plasmanm_qmlplugins DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/networkmanagement)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/networkmanagement)
