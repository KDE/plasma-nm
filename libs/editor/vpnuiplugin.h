/*
    SPDX-FileCopyrightText: 2008 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_VPN_UI_PLUGIN_H
#define PLASMA_NM_VPN_UI_PLUGIN_H

#include <QMessageBox>
#include <QObject>
#include <QVariant>

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GenericTypes>
#include <NetworkManagerQt/VpnSetting>

#include <KPluginFactory>

#include "settingwidget.h"

/**
 * Plugin for UI elements for VPN configuration
 */
class Q_DECL_EXPORT VpnUiPlugin : public QObject
{
    Q_OBJECT
public:
    enum ErrorType { NoError, NotImplemented, Error };

    explicit VpnUiPlugin(QObject *parent = nullptr, const QVariantList & = QVariantList());
    ~VpnUiPlugin() override;

    virtual SettingWidget *widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr) = 0;
    virtual SettingWidget *askUser(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent = nullptr) = 0;

    /**
     * Suggested file name to save the exported connection configuration.
     * Try not to use space, parenthesis, or any other Unix unfriendly file name character.
     */
    virtual QString suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const = 0;
    /**
     * File extension to be used in QFileDialog when selecting the file to import.
     * The format is: *.<extension> [*.<extension> ...]. For instance: '*.pcf'
     */
    virtual QString supportedFileExtensions() const = 0;

    /**
     * If the plugin does not support fileName's extension it must just return an empty QVariantList.
     * If it supports the extension and import has failed it must set mError with VpnUiPlugin::Error
     * and mErrorMessage with a custom error message before returning an empty QVariantList.
     */
    virtual NMVariantMapMap importConnectionSettings(const QString &fileName) = 0;
    virtual bool exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName) = 0;

    virtual QMessageBox::StandardButtons suggestedAuthDialogButtons() const;
    ErrorType lastError() const;
    QString lastErrorMessage();

    struct LoadResult {
        VpnUiPlugin *plugin = nullptr;
        QString error;

        operator bool() const
        {
            return plugin != nullptr;
        }
    };

    static KPluginFactory::Result<VpnUiPlugin> loadPluginForType(QObject *parent, const QString &serviceType);

protected:
    ErrorType mError;
    QString mErrorMessage;
};

#endif // PLASMA_NM_VPN_UI_PLUGIN_H
