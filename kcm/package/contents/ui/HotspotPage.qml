/*
    Copyright 2020 Carson Black <uhhadd@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.6
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5 as QQC2

import org.kde.kcm 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.9 as Kirigami

SimpleKCM {
    id: root

    title: i18n("Hotspot")

    PlasmaNM.Configuration {
        id: configuration
    }

    Kirigami.FormLayout {
        QQC2.TextField {
            id: hotspotName
            Kirigami.FormData.label: i18n("Hotspot name:")
            onTextChanged: configuration.hotspotName = hotspotName.text
            Component.onCompleted: text = configuration.hotspotName
        }

        QQC2.TextField {
            id: hotspotPassword
            Kirigami.FormData.label: i18n("Hotspot password:")
            validator: RegExpValidator {
                regExp: if (useApMode) {
                            /^$|^(?:.{8,64}){1}$/
                        } else {
                            /^$|^(?:.{5}|[0-9a-fA-F]{10}|.{13}|[0-9a-fA-F]{26}){1}$/
                        }
            }

            onAcceptableInputChanged: configuration.hotspotPassword = hotspotPassword.text
            Component.onCompleted: text = configuration.hotspotPassword
        }
    }
}