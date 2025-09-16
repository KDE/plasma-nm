# SPDX-FileCopyrightText: 2018 Martin Kacej <m.kacej@atlas.sk>
# SPDX-License-Identifier: LGPL-2.0-or-later

kcmutils_add_qml_kcm(kcm_mobile_wired SOURCES wiredsettings.cpp)
target_link_libraries(kcm_mobile_wired PRIVATE
    Qt::DBus
    Qt::Gui
    Qt::Quick
    Qt::Qml
    KF6::I18n
    KF6::NetworkManagerQt
    Plasma::Plasma
    KF6::KCMUtilsQuick
)

