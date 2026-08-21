set(SETTINGS_SRCS
 	settings/wifisecurities/security8021xsetting.cpp
 	settings/wifisecurities/security8021xsetting.h
	settings/wifisecurities/wifisecuritysetting.cpp
	settings/wifisecurities/wifisecuritysetting.h
  settings/stringlistmodel.cpp
  settings/stringlistmodel.h
	settings/wifistatus/connectionstatus.h
	settings/wifistatus/connectionstatus.cpp
  settings/generalSettings/generalsettings.cpp
  settings/generalSettings/generalsettings.h
  settings/generalSettings/advancedpermissionsmodel.h
  settings/generalSettings/advancedpermissionsmodel.cpp
  settings/wificonnectionsettings/wifisetting.cpp
  settings/wificonnectionsettings/wifisetting.h
  settings/ipv4settings/ipv4settings.cpp
  settings/ipv4settings/ipv4settings.h
  settings/ipv4settings/ipv4routetablemodel.cpp
  settings/ipv4settings/ipv4routetablemodel.h
  settings/ipv6settings/ipv6settings.cpp
  settings/ipv6settings/ipv6settings.h
  settings/ipv6settings/ipv6routetablemodel.cpp
  settings/ipv6settings/ipv6routetablemodel.h
)

set(QML_SRCS
  qml/wifisecurity/authentication/enterprise/eap/EapTls.qml
  qml/wifisecurity/authentication/enterprise/eap/EapTtls.qml
  qml/wifisecurity/authentication/enterprise/eap/EapMd5.qml
  qml/wifisecurity/authentication/enterprise/eap/EapLeap.qml
  qml/wifisecurity/authentication/enterprise/eap/EapFast.qml
  qml/wifisecurity/authentication/enterprise/eap/EapPeap.qml
  qml/wifisecurity/authentication/enterprise/eap/EapPwd.qml

  qml/PasswordField.qml
  qml/ConfigurationDialog.qml
  qml/wifisecurity/WifiSecurityForm.qml
  qml/wifisecurity/authentication/WpaPersonal.qml
  qml/wifisecurity/authentication/WepKey.qml
  qml/wifisecurity/authentication/Leap.qml
  qml/wifisecurity/authentication/Owe.qml

  qml/wifisecurity/authentication/enterprise/DynamicWep.qml
  qml/wifisecurity/authentication/enterprise/WpaEnterprise.qml
  qml/wifisecurity/authentication/enterprise/Wpa3Enterprise192.qml
  qml/wifisecurity/DnsList.qml
  qml/wifisecurity/AlternativeSubjectMatch.qml

  qml/components/Wireless.qml

  qml/PasswordPromptDialog.qml

  qml/AddConnectionDialog.qml
  qml/wifistatus/ConnectionStatusForm.qml

  qml/generalSettings/GeneralSettings.qml
  qml/generalSettings/AdvancedPermissionsDialog.qml
  qml/wificonnectionsettings/WifiConnectionSettings.qml

  qml/ipv4/IPv4Settings.qml
  qml/ipv4/IPv4Advance.qml
  qml/ipv4/IPv4Routes.qml
  qml/ipv4/SearchDomainList.qml

  qml/ipv6/IPv6Settings.qml
  qml/ipv6/IPv6Routes.qml
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
      QML_FILES ${QML_SRCS}
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
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/wifistatus
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/generalSettings
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/wificonnectionsettings
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/ipv4settings
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/ipv6settings
)

target_compile_definitions(plasmanm_editorqml PRIVATE BROADBANDPROVIDER_DATABASE=\"${BROADBANDPROVIDER_DATABASE}\")

ecm_finalize_qml_module(plasmanm_editorqml)

install(TARGETS plasmanm_editorqml ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
