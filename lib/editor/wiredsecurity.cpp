/*
    Copyright (c) 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#include "wiredsecurity.h"
#include "ui_wiredsecurity.h"

#include <QDebug>

WiredSecurity::WiredSecurity(const NetworkManager::Security8021xSetting::Ptr &setting8021x, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting8021x, parent, f),
    m_ui(new Ui::WiredSecurity),
    m_8021xSetting(setting8021x)
{
    m_ui->setupUi(this);

    m_8021xWidget = new Security8021x(m_8021xSetting, false, this);
    m_8021xWidget->setDisabled(true);

    m_ui->verticalLayout->addWidget(m_8021xWidget);

    loadConfig(setting8021x);

    connect(m_ui->use8021X, SIGNAL(toggled(bool)), m_8021xWidget, SLOT(setEnabled(bool)));
}

WiredSecurity::~WiredSecurity()
{
    delete m_ui;
}

bool WiredSecurity::enabled8021x() const
{
    if (m_ui->use8021X->checkState() == Qt::Checked) {
        return true;
    }

    return false;
}

void WiredSecurity::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    if (!setting->isNull()) {
        m_8021xWidget->setEnabled(true);
        m_ui->use8021X->setChecked(true);
    }
}

QVariantMap WiredSecurity::setting(bool agentOwned) const
{
    if (m_ui->use8021X->isChecked())
        return m_8021xWidget->setting(agentOwned);

    return QVariantMap();
}
