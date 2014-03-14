/*
Copyright 2011 Lamarque Souza <lamarque@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PLASMA_NM_PIN_DIALOG_H
#define PLASMA_NM_PIN_DIALOG_H

#include <QWidget>

#include <KDialog>
#include <KLocale>
#include <KPushButton>
#include <Solid/Device>

#include "config.h"

#include <ModemManagerQt/modem.h>
#include "ui_pinwidget.h"

class PinWidget;

class PinDialog : public KDialog
{
    Q_OBJECT
public:
    enum Type {
        SimPin,
        SimPin2,
        SimPuk,
        SimPuk2,
        ModemServiceProviderPin,
        ModemServiceProviderPuk,
        ModemNetworkPin,
        ModemNetworkPuk,
        ModemPin,
        ModemCorporatePin,
        ModemCorporatePuk,
        ModemPhFsimPin,
        ModemPhFsimPuk,
        ModemNetworkSubsetPin,
        ModemNetworkSubsetPuk
    };
    enum ErrorCode {PinCodeTooShort, PinCodesDoNotMatch, PukCodeTooShort};

    explicit PinDialog(ModemManager::Modem *modem, const Type type = SimPin, QWidget *parent=0);
    ~PinDialog();

    Type type() const;
    QString pin() const;
    QString pin2() const;
    QString puk() const;

public Q_SLOTS:
    void accept();

private Q_SLOTS:
    void chkShowPassToggled(bool on);
    void modemRemoved(const QString &udi);

private:
    void showErrorMessage(const PinDialog::ErrorCode);
    bool isPukDialog() const;
    bool isPinDialog() const;
    Ui::PinWidget * ui;
    QLabel* pixmapLabel;
    QString m_name;
    Type m_type;
    QString m_udi;
};

#endif // PLASMA_NM_PIN_DIALOG_H
