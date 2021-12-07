#KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-kcm\")

set(kcm_networkmanagement_PART_SRCS
    kcm.cpp
    kcm.h
)
ecm_qt_declare_logging_category(kcm_networkmanagement_PART_SRCS HEADER plasma_nm_kcm.h IDENTIFIER PLASMA_NM_KCM_LOG CATEGORY_NAME org.kde.plasma.nm.kcm DESCRIPTION "Plasma NM (kcm)" EXPORT PLASMANM)

ki18n_wrap_ui(kcm_networkmanagement_PART_SRCS
    kcm.ui
)

add_library(kcm_networkmanagement MODULE ${kcm_networkmanagement_PART_SRCS})
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
install(TARGETS kcm_networkmanagement  DESTINATION ${KDE_INSTALL_PLUGINDIR} )

########### install files ###############

install( FILES kcm_networkmanagement.desktop  DESTINATION  ${KDE_INSTALL_KSERVICES5DIR} )
install( FILES
    qml/AddConnectionDialog.qml
    qml/ConfigurationDialog.qml
    qml/ConnectionItem.qml
    qml/ListItem.qml
    qml/main.qml

    DESTINATION ${KDE_INSTALL_DATADIR}/kcm_networkmanagement/qml)
