/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_VPN_VIRTUAL_BASE_H
#define PLASMA_NM_VPN_VIRTUAL_BASE_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QStringList>

class PLASMANM_EDITORQML_EXPORT VpnSettingBase : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
    ~VpnSettingBase() override;
    virtual QString serviceType() const = 0;
    virtual bool isValid() const = 0;

Q_SIGNALS:
    void validChanged();
};

#endif
