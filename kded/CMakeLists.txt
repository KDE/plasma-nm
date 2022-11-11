add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-kded\")

add_library(kded_networkmanagement MODULE)

target_sources(kded_networkmanagement PRIVATE
    bluetoothmonitor.cpp
    connectivitymonitor.cpp
    notification.cpp
    monitor.cpp
    passworddialog.cpp
    secretagent.cpp
    service.cpp
    bluetoothmonitor.h
    connectivitymonitor.h
    notification.h
    monitor.h
    passworddialog.h
    secretagent.h
    service.h
    modemmonitor.cpp
    modemmonitor.h
    pindialog.cpp
    pindialog.h
)

ki18n_wrap_ui(kded_networkmanagement
    passworddialog.ui
    pinwidget.ui
)

ecm_qt_declare_logging_category(kded_networkmanagement HEADER plasma_nm_kded.h IDENTIFIER PLASMA_NM_KDED_LOG CATEGORY_NAME org.kde.plasma.nm.kded DESCRIPTION "Plasma NM (kded)" EXPORT PLASMANM)


set_target_properties(kded_networkmanagement PROPERTIES OUTPUT_NAME networkmanagement)

target_link_libraries(kded_networkmanagement
    plasmanm_internal
    plasmanm_editor
    KF5::ConfigCore
    KF5::DBusAddons
    KF5::I18n
    KF5::Notifications
    KF5::Service
    KF5::Solid
    KF5::Wallet
    KF5::WindowSystem
    KF5::KIOGui
    KF5::ModemManagerQt
)

install(TARGETS kded_networkmanagement DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/kded)

install(FILES networkmanagement.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
