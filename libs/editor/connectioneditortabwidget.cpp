/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "connectioneditortabwidget.h"
#include "ui_connectioneditortabwidget.h"

#include <QTimer>

ConnectionEditorTabWidget::ConnectionEditorTabWidget(const NetworkManager::ConnectionSettings::Ptr &connection,
                                                     bool focusWifiSecurityUsername,
                                                     QWidget *parent,
                                                     Qt::WindowFlags f)
    : ConnectionEditorBase(connection, parent, f)
    , m_ui(new Ui::ConnectionEditorTabWidget)
    , m_focusWifiSecurityUsername(focusWifiSecurityUsername)
{
    m_ui->setupUi(this);
    m_ui->tabWidget->setUsesScrollButtons(false);

    connect(m_ui->connectionName, &QLineEdit::textChanged, this, &ConnectionEditorTabWidget::settingChanged);

    initialize();

    initializeTabWidget(connection);
}

void ConnectionEditorTabWidget::setConnection(const NetworkManager::ConnectionSettings::Ptr &connection)
{
    // Clear all existing tabs before re-initializing
    m_ui->tabWidget->clear();

    // Init tabs first as we need to set current tab after that
    ConnectionEditorBase::setConnection(connection);

    initializeTabWidget(connection);
}

ConnectionEditorTabWidget::~ConnectionEditorTabWidget()
{
    delete m_ui;
}

void ConnectionEditorTabWidget::addWidget(QWidget *widget, const QString &text)
{
    m_ui->tabWidget->addTab(widget, text);
}

QString ConnectionEditorTabWidget::connectionName() const
{
    return m_ui->connectionName->text();
}

void ConnectionEditorTabWidget::initializeTabWidget(const NetworkManager::ConnectionSettings::Ptr &connection)
{
    if (connection->id().isEmpty()) {
        m_ui->connectionName->setText(i18n("New %1 connection", connection->typeAsString(connection->connectionType())));
    } else {
        m_ui->connectionName->setText(connection->id());
    }

    if (m_focusWifiSecurityUsername && wifiTabIndex() >= 0) {
        m_ui->tabWidget->setCurrentIndex(wifiTabIndex());
        QTimer::singleShot(0, this, &ConnectionEditorBase::focusWifiSecurityUsername);
    } else {
        // Set current tab to "Status"
        m_ui->tabWidget->setCurrentIndex(0);
    }
}

#include "moc_connectioneditortabwidget.cpp"
