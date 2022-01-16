#KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-kcm\")

kcoreaddons_add_plugin(kcm_networkmanagement INSTALL_NAMESPACE "plasma/kcms/systemsettings_qwidgets")

target_sources(kcm_networkmanagement PRIVATE
    kcm.cpp
    kcm.h
)
ecm_qt_declare_logging_category(kcm_networkmanagement HEADER plasma_nm_kcm.h IDENTIFIER PLASMA_NM_KCM_LOG CATEGORY_NAME org.kde.plasma.nm.kcm DESCRIPTION "Plasma NM (kcm)" EXPORT PLASMANM)
ki18n_wrap_ui(kcm_networkmanagement
    kcm.ui
)
target_link_libraries(kcm_networkmanagement
    plasmanm_internal
    plasmanm_editor
    KF5::ConfigWidgets
    KF5::Declarative
    KF5::I18n
    KF5::Service
    Qt::Quick
    Qt::QuickWidgets
)

########### install files ###############

install( FILES kcm_networkmanagement.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES
    qml/AddConnectionDialog.qml
    qml/ConfigurationDialog.qml
    qml/ConnectionItem.qml
    qml/ListItem.qml
    qml/main.qml

    DESTINATION ${KDE_INSTALL_DATADIR}/kcm_networkmanagement/qml)
