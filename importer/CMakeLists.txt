# SPDX-FileCopyrightText: 2024 Kai Uwe Broulik <kde@broulik.de>
# SPDX-License-Identifier: BSD-3-Clause

add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-importer\")

add_executable(plasmanm-importer main.cpp)

target_compile_definitions(plasmanm-importer PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(plasmanm-importer
    Qt6::Qml # qqmlregistration in Handler
    Qt6::Widgets
    KF6::I18n
    KF6::KIOGui
    KF6::NetworkManagerQt
    plasmanm_internal
    plasmanm_editor
)

install(TARGETS plasmanm-importer ${KF_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.kde.plasma.networkmanagement.importer.desktop DESTINATION ${KDE_INSTALL_APPDIR})

