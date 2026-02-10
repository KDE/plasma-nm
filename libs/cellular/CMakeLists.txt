# SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

add_library(plasmanm_cellular SHARED)
generate_export_header(plasmanm_cellular)

ecm_qt_declare_logging_category(plasmanm_cellular HEADER plasma_nm_cellular.h IDENTIFIER PLASMA_NM_CELLULAR_LOG CATEGORY_NAME org.kde.plasma.nm.cellular DESCRIPTION "Plasma NM (cellular)" EXPORT PLASMANM)

target_sources(plasmanm_cellular PRIVATE
    cellularmodem.cpp
    cellularmodem.h
    cellularmodemdetails.cpp
    cellularmodemdetails.h
    cellularsim.cpp
    cellularsim.h
    cellularconnectionprofile.cpp
    cellularconnectionprofile.h
    cellularmodemlist.cpp
    cellularmodemlist.h
)

target_include_directories(plasmanm_cellular
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(plasmanm_cellular
PUBLIC
    KF6::NetworkManagerQt
    KF6::ModemManagerQt
    QCoro::Core
PRIVATE
    KF6::I18n
    KF6::CoreAddons
    QCoro::DBus
    Qt::Xml
    Qt::Qml
    Qt::DBus
    plasmanm_editor
)

ecm_add_qml_module(plasmanm_cellular URI org.kde.plasma.networkmanagement.cellular GENERATE_PLUGIN_SOURCE)
ecm_finalize_qml_module(plasmanm_cellular)

install(TARGETS plasmanm_cellular ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
