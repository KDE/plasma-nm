add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_sshui\")



add_library(plasmanetworkmanagement_sshui MODULE)
target_sources(plasmanetworkmanagement_sshui PRIVATE
   ssh.cpp
   sshwidget.cpp
   sshauth.cpp
   )
ki18n_wrap_ui(plasmanetworkmanagement_sshui sshwidget.ui sshadvanced.ui sshauth.ui)

kcoreaddons_desktop_to_json(plasmanetworkmanagement_sshui plasmanetworkmanagement_sshui.desktop)

target_link_libraries(plasmanetworkmanagement_sshui
    plasmanm_internal
    plasmanm_editor
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOWidgets
    KF5::WidgetsAddons
)

install(TARGETS plasmanetworkmanagement_sshui  DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/network/vpn)
