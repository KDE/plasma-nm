project (kcm_mobile_cellural)

set (celluralsettings_SRCS celluralsettings.cpp)

add_library(kcm_mobile_cellural MODULE ${celluralsettings_SRCS})

target_link_libraries(kcm_mobile_cellural
    Qt5::DBus
    Qt5::Gui
    Qt5::Quick
    Qt5::Qml
    KF5::Plasma
    KF5::I18n
    KF5::QuickAddons
)

kcoreaddons_desktop_to_json(kcm_mobile_cellural "mobile_cellural.desktop")

install(FILES mobile_cellural.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kcm_mobile_cellural DESTINATION ${PLUGIN_INSTALL_DIR}/kcms)
kpackage_install_package(package kcm_mobile_cellural kcms)
