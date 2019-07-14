/*
    Copyright 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

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

import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.3 as Kirigami
import org.kde.prison 1.0 as Prison

Window
{
    id: window
    color: Qt.rgba(0, 0, 0, 0.3)
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    property alias content: dataInput.content

    MouseArea {
        anchors.fill: parent

        onClicked: {
            window.visible = false
            window.destroy()
        }

        Prison.Barcode {
            anchors {
                fill: parent
                margins: Kirigami.Units.largeSpacing * 4
            }

            id: dataInput
            barcodeType: "QRCode"
        }
    }
}
