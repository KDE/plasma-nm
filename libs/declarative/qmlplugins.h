/*
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_QML_PLUGINS_H
#define PLASMA_NM_QML_PLUGINS_H

#include <QQmlExtensionPlugin>

class QmlPlugins : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
    public:
        void registerTypes(const char * uri) override;
};

#endif // PLASMA_NM_QML_PLUGINS_H
