#KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-kcm\")

set(kcm_networkmanagement_SRCS
    kcm.cpp
)

add_library(kcm_networkmanagement MODULE ${kcm_networkmanagement_SRCS})

target_link_libraries(kcm_networkmanagement
    plasmanm_internal
    plasmanm_editor
    Qt5::Quick
    Qt5::QuickWidgets
    KF5::Completion
    KF5::ConfigWidgets
    KF5::CoreAddons
    KF5::Declarative
    KF5::I18n
    KF5::NetworkManagerQt
    KF5::Service
    KF5::QuickAddons
    KF5::WindowSystem
)
install(TARGETS kcm_networkmanagement  DESTINATION ${KDE_INSTALL_PLUGINDIR} )

kcoreaddons_desktop_to_json(kcm_networkmanagement "kcm_networkmanagement.desktop" SERVICE_TYPES kcmodule.desktop)

########### install files ###############
install(TARGETS kcm_networkmanagement DESTINATION ${PLUGIN_INSTALL_DIR}/kcms)
install( FILES kcm_networkmanagement.desktop  DESTINATION  ${KDE_INSTALL_KSERVICES5DIR})

kpackage_install_package(package kcm_networkmanagement kcms)
