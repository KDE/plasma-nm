add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-kcm-qml\")


kcmutils_add_qml_kcm(kcm_networkmanagement_qml
    URI "org.kde.plasma.networkmanagement.kcm"
    INSTALL_NAMESPACE "plasma/kcms/systemsettings"
)

target_sources(kcm_networkmanagement_qml PRIVATE
    kcm.cpp
    kcm.h
)

ecm_qt_declare_logging_category(kcm_networkmanagement_qml
    HEADER plasma_nm_kcm_qml.h
    IDENTIFIER PLASMA_NM_KCM_QML_LOG
    CATEGORY_NAME org.kde.plasma.nm.kcm_qml
    DESCRIPTION "Plasma NM (kcm-qml)"
    EXPORT PLASMANM
)

target_link_libraries(kcm_networkmanagement_qml
    PRIVATE
        plasmanm_internal
        plasmanm_editorqml
        KF6::KCMUtils
        KF6::KCMUtilsQuick
        KF6::I18n
        KF6::Service
        Qt::Quick
)
