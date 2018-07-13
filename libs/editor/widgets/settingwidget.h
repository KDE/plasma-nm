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

#ifndef SETTING_WIDGET_H
#define SETTING_WIDGET_H

#include <QWidget>
#include <NetworkManagerQt/Setting>

#include <KAcceleratorManager>

class Q_DECL_EXPORT SettingWidget : public QWidget
{
    Q_OBJECT
public:
    class EnumPasswordStorageType
    {
    public:
        enum PasswordStorageType {Store = 0, AlwaysAsk, NotRequired};
    };

    explicit SettingWidget(const NetworkManager::Setting::Ptr &setting = NetworkManager::Setting::Ptr(), QWidget* parent = nullptr, Qt::WindowFlags f = {});
    ~SettingWidget() override;

    virtual void loadConfig(const NetworkManager::Setting::Ptr &setting);
    virtual void loadSecrets(const NetworkManager::Setting::Ptr &setting);

    virtual QVariantMap setting() const = 0;

    // Do not forget to call this function in the inherited class once initialized
    void watchChangedSetting();

    QString type() const;

    virtual bool isValid() const { return true; }

protected Q_SLOTS:
    void slotWidgetChanged();

Q_SIGNALS:
    void validChanged(bool isValid);
    void settingChanged();
private:
    QString m_type;
};

#endif // SETTING_WIDGET_H
