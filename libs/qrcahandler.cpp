/*
 * SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "qrcahandler.h"

#include <QUrl>

#include <KApplicationTrader>
#include <KNotificationJobUiDelegate>
#include <KService>
#include <KSycoca>

#include <KIO/ApplicationLauncherJob>

using namespace Qt::Literals::StringLiterals;

// Wifi scanner has its own desktop file but is still part of qrca.

static KService::Ptr qrcaWifiService()
{
    return KService::serviceByDesktopName(u"org.kde.qrca.wifi"_s);
}

QrcaHandler::QrcaHandler(QObject *parent)
    : QObject(parent)
{
    connect(KSycoca::self(), &KSycoca::databaseChanged, this, &QrcaHandler::update);
    update();
}

QrcaHandler::~QrcaHandler() = default;

bool QrcaHandler::available() const
{
    return m_available;
}

void QrcaHandler::launch()
{
    auto *job = new KIO::ApplicationLauncherJob{qrcaWifiService()};
    job->setUiDelegate(new KNotificationJobUiDelegate{KJobUiDelegate::AutoErrorHandlingEnabled});
    job->start();
}

void QrcaHandler::update()
{
    const bool available = qrcaWifiService();
    if (m_available != available) {
        m_available = available;
        Q_EMIT availableChanged();
    }
}

#include "moc_qrcahandler.cpp"
