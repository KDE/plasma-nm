/*
 *   Copyright 2020 Tobias Fella <fella@posteo.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2 as Controls
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.10 as Kirigami
import org.kde.kcm 1.2

SimpleKCM {

    PlasmaNM.Handler {
        id: handler
    }

    Kirigami.FormLayout {
        Controls.Switch {
            id: hotspotToggle
            Kirigami.FormData.label: i18n("Enabled:")
            onToggled: {
                if (hotspotToggle.checked) {
                    handler.createHotspot()
                } else {
                    handler.stopHotspot()
                }
            }
        }

        Controls.TextField {
            id: hotspotName
            Kirigami.FormData.label: i18n("SSID:")
            text: PlasmaNM.Configuration.hotspotName
        }

        Kirigami.PasswordField {
            id: hotspotPassword
            Kirigami.FormData.label: i18n("Password:")
            text: PlasmaNM.Configuration.hotspotPassword
        }

        Controls.Button {
            text: i18n("Save")
            onClicked: {
                PlasmaNM.Configuration.hotspotName = hotspotName.text
                PlasmaNM.Configuration.hotspotPassword = hotspotPassword.text
                if (hotspotToggle.checked) {
                    handler.stopHotspot()
                    handler.createHotspot()
                }
            }
        }
    }
}
