set (hotspotsettings_SRCS hotspotsettings.cpp)

add_library(kcm_mobile_hotspot MODULE ${hotspotsettings_SRCS})

target_link_libraries(kcm_mobile_hotspot
    Qt::Quick
    Qt::Qml
    KF5::I18n
    KF5::CoreAddons
    KF5::QuickAddons
)

install(TARGETS kcm_mobile_hotspot DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)
kpackage_install_package(package kcm_mobile_hotspot kcms)
