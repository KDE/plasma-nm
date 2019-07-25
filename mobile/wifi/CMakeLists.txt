project (kcm_mobile_wifi)

set (wifisettings_SRCS wifisettings.cpp)

add_library(kcm_mobile_wifi MODULE ${wifisettings_SRCS})

target_link_libraries(kcm_mobile_wifi
    Qt5::DBus
    Qt5::Gui
    Qt5::Quick
    Qt5::Qml
    KF5::I18n
    KF5::NetworkManagerQt
    KF5::Plasma
    KF5::QuickAddons
)

kcoreaddons_desktop_to_json(kcm_mobile_wifi "wifisettings.desktop")

install(FILES wifisettings.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kcm_mobile_wifi DESTINATION ${PLUGIN_INSTALL_DIR}/kcms)
kpackage_install_package(package kcm_mobile_wifi kcms)
