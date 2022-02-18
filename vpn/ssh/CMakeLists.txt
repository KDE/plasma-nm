add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_sshui\")



add_library(plasmanetworkmanagement_sshui MODULE)
target_sources(plasmanetworkmanagement_sshui PRIVATE
   ssh.cpp
   sshwidget.cpp
   sshauth.cpp
   ssh.h
   sshwidget.h
   sshauth.h
   )
ki18n_wrap_ui(plasmanetworkmanagement_sshui sshwidget.ui sshadvanced.ui sshauth.ui)

target_link_libraries(plasmanetworkmanagement_sshui
    plasmanm_internal
    plasmanm_editor
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOWidgets
    KF5::WidgetsAddons
)

install(TARGETS plasmanetworkmanagement_sshui  DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/network/vpn)
