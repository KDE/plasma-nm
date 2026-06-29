set(SETTINGS_SRCS
 	settings/wifisecurities/security8021xsetting.cpp
 	settings/wifisecurities/security8021xsetting.h
	settings/wifisecurities/wifisecuritysetting.cpp
	settings/wifisecurities/wifisecuritysetting.h
)


add_library(plasmanm_editorqml SHARED ${SETTINGS_SRCS})

generate_export_header(plasmanm_editorqml
    BASE_NAME plasmanm_editorqml
    EXPORT_FILE_NAME plasmanm_editorqml_export.h
)

ecm_add_qml_module(plasmanm_editorqml
      URI "org.kde.plasma.networkmanagement.editorqml"
      VERSION 1.0
      GENERATE_PLUGIN_SOURCE
      DEPENDENCIES
          QtQuick
          QtQuick.Controls
          QtQuick.Layouts
          QtQuick.Dialogs
          org.kde.kirigami
          "org.kde.plasma.networkmanagement"
)

ecm_qt_declare_logging_category(plasmanm_editorqml
    HEADER plasma_nm_editorqml.h
    IDENTIFIER PLASMA_NM_EDITORQML_LOG
    CATEGORY_NAME org.kde.plasma.nm.editorqml
    DESCRIPTION "Plasma NM (editorqml)"
    EXPORT PLASMANM
)

target_link_libraries(plasmanm_editorqml
      PUBLIC
          Qt6::Core
          Qt6::DBus
          Qt6::Network
          Qt6::Qml
          KF6::NetworkManagerQt
          KF6::ModemManagerQt
          QCoro6::Core
          QCoro6::DBus
      PRIVATE
          Qt6::Quick
          OpenSSL::Crypto
          Qt6Keychain::Qt6Keychain
          KF6::I18n
          KF6::KIOWidgets
          KF6::Notifications
          KF6::Solid
          KF6::WindowSystem
          plasmanm_internal
)
 
target_include_directories(plasmanm_editorqml
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/settings
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/wifisecurities
)

target_compile_definitions(plasmanm_editorqml PRIVATE BROADBANDPROVIDER_DATABASE=\"${BROADBANDPROVIDER_DATABASE}\")

ecm_finalize_qml_module(plasmanm_editorqml)

install(TARGETS plasmanm_editorqml ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
