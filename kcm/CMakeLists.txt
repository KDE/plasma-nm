# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-kcm\")

set(kcm_networkmanagement_PART_SRCS
    kcm.cpp
)

ki18n_wrap_ui(kcm_networkmanagement_PART_SRCS
    kcm.ui
)

add_library(kcm_networkmanagement MODULE ${kcm_networkmanagement_PART_SRCS})
target_link_libraries(kcm_networkmanagement
#     Qt5::DBus
    Qt5::Quick
    Qt5::QuickWidgets
    Qt5::UiTools
    KF5::Completion
    KF5::ConfigWidgets
    KF5::Declarative
    KF5::I18n
#     KF5::NewStuff
    KF5::WindowSystem
    KF5::Service
)
install(TARGETS kcm_networkmanagement  DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES kcm_networkmanagement.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES
    qml/main.qml
    qml/ConnectionItem.qml
    qml/DeviceItem.qml
    qml/DevicePage.qml
    DESTINATION ${DATA_INSTALL_DIR}/kcm_networkmanagement/qml)