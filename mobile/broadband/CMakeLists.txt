project (kcm_mobile_broadband)

set (mobilebroadbandsettings_SRCS mobilebroadbandsettings.cpp)

add_library(kcm_mobile_broadband MODULE ${mobilebroadbandsettings_SRCS})

target_link_libraries(kcm_mobile_broadband
    Qt5::DBus
    Qt5::Gui
    Qt5::Quick
    Qt5::Qml
    KF5::Plasma
    KF5::I18n
    KF5::QuickAddons
)

kcoreaddons_desktop_to_json(kcm_mobile_broadband "mobilebroadbandsettings.desktop")

install(FILES mobilebroadbandsettings.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kcm_mobile_broadband DESTINATION ${PLUGIN_INSTALL_DIR}/kcms)
kpackage_install_package(package kcm_mobile_broadband kcms)
