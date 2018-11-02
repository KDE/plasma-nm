/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#include "settingwidget.h"
#include "bssidcombobox.h"
#include "hwaddrcombobox.h"
#include "ssidcombobox.h"
#include "passwordfield.h"

#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTableView>

#include <KUrlRequester>

SettingWidget::SettingWidget(const NetworkManager::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    QWidget(parent, f),
    m_type(setting->name())
{
}

SettingWidget::~SettingWidget()
{
}

void SettingWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_UNUSED(setting);
}

void SettingWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    Q_UNUSED(setting);
}

void SettingWidget::watchChangedSetting()
{
    // Attempt to connect to all widgets representing various configurations
    // to notify about setting change

    /************ Qt Widgets ************/

    // Connect all QLineEdit widgets
    QList<QLineEdit *> lineEdits = findChildren<QLineEdit *>();
    Q_FOREACH (QLineEdit *lineedit, lineEdits) {
        connect(lineedit, &QLineEdit::textChanged, this, &SettingWidget::settingChanged);
    }

    // Connect all QComboBox widgets
    QList<QComboBox *> comboboxes = findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *combobox, comboboxes) {
        connect(combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingWidget::settingChanged);
        connect(combobox, &QComboBox::currentTextChanged, this, &SettingWidget::settingChanged);
    }

    // Connect all QCheckBox widgets
    QList<QCheckBox *> checkboxes = findChildren<QCheckBox *>();
    Q_FOREACH (QCheckBox *checkbox, checkboxes) {
        connect(checkbox, &QCheckBox::stateChanged, this, &SettingWidget::settingChanged);
    }

    // Connect all QPushButton widgets
    QList<QPushButton *> pushbuttons = findChildren<QPushButton *>();
    Q_FOREACH (QPushButton *pushbutton, pushbuttons) {
        connect(pushbutton, &QPushButton::clicked, this, &SettingWidget::settingChanged);
    }

    // Connect all QSpinBox widgets
    QList<QSpinBox *> spinboxes = findChildren<QSpinBox *>();
    Q_FOREACH (QSpinBox *spinbox, spinboxes) {
        connect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingWidget::settingChanged);
    }

    // Connect all KUrlRequester widgets
    QList<KUrlRequester *> urlrequesters = findChildren<KUrlRequester *>();
    Q_FOREACH (KUrlRequester *urlrequester, urlrequesters) {
        connect(urlrequester, &KUrlRequester::textChanged, this, &SettingWidget::settingChanged);
        connect(urlrequester, &KUrlRequester::urlSelected, this, &SettingWidget::settingChanged);
    }

    // Connect all QTableView widgets
    QList<QTableView *> tableviews = findChildren<QTableView *>();
    Q_FOREACH(QTableView *tableview, tableviews) {
        connect(tableview, &QTableView::clicked, this, &SettingWidget::settingChanged);
    }

    /********** OUR CUSTOM WIDGETS **********/
    // Connect all PasswordField widgets
    QList<PasswordField *> passwordfields = findChildren<PasswordField *>();
    Q_FOREACH (PasswordField *passwordfield, passwordfields) {
        connect(passwordfield, &PasswordField::textChanged, this, &SettingWidget::settingChanged);
        connect(passwordfield, &PasswordField::passwordOptionChanged, this, &SettingWidget::settingChanged);
    }

    // Connect all HwAddrComboBox widgets
    QList<HwAddrComboBox *> hwAddrcomboboxes = findChildren<HwAddrComboBox *>();
    Q_FOREACH (HwAddrComboBox *combobox, hwAddrcomboboxes) {
        connect(combobox, &HwAddrComboBox::hwAddressChanged, this, &SettingWidget::settingChanged);
    }

    // Connect all SssidComboBox widgets
    QList<SsidComboBox *> ssidcomboboxes = findChildren<SsidComboBox *>();
    Q_FOREACH (SsidComboBox *combobox, ssidcomboboxes) {
        connect(combobox, &SsidComboBox::ssidChanged, this, &SettingWidget::settingChanged);
    }

    // Connect all BssidComboBox widgets
    QList<BssidComboBox *> bssidcomboboxes = findChildren<BssidComboBox *>();
    Q_FOREACH (BssidComboBox *combobox, bssidcomboboxes) {
        connect(combobox, &BssidComboBox::bssidChanged, this, &SettingWidget::settingChanged);
    }
}

QString SettingWidget::type() const
{
    return m_type;
}

void SettingWidget::slotWidgetChanged()
{
    Q_EMIT validChanged(isValid());
}
