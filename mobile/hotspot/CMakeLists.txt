set (hotspotsettings_SRCS hotspotsettings.cpp)

add_library(kcm_mobile_hotspot MODULE ${hotspotsettings_SRCS})

target_link_libraries(kcm_mobile_hotspot
    Qt::Quick
    Qt::Qml
    KF5::I18n
    KF5::CoreAddons
    KF5::QuickAddons
)

kcoreaddons_desktop_to_json(kcm_mobile_hotspot "package/metadata.desktop")

install(FILES package/metadata.desktop RENAME hotspotsettings.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(TARGETS kcm_mobile_hotspot DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)
kpackage_install_package(package kcm_mobile_hotspot kcms)
