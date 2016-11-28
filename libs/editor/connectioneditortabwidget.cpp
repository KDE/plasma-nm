/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

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

#include "connectioneditortabwidget.h"
#include "ui_connectioneditortabwidget.h"

ConnectionEditorTabWidget::ConnectionEditorTabWidget(const NetworkManager::ConnectionSettings::Ptr &connection,
                                                     QWidget *parent, Qt::WindowFlags f)
    : ConnectionEditorBase(connection, parent, f)
    , m_ui(new Ui::ConnectionEditorTabWidget)
{
    m_ui->setupUi(this);
    m_ui->tabWidget->setUsesScrollButtons(false);

    connect(m_ui->connectionName, &QLineEdit::textChanged, this, &ConnectionEditorTabWidget::settingChanged);

    initialize();

    initializeTabWidget(connection);
}

void ConnectionEditorTabWidget::setConnection(const NetworkManager::ConnectionSettings::Ptr &connection)
{
    // Init tabs first as we need to set current tab after that
    ConnectionEditorBase::setConnection(connection);

    initializeTabWidget(connection);
}

ConnectionEditorTabWidget::~ConnectionEditorTabWidget()
{
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

    // Set current tab to the connection specific configuration
    m_ui->tabWidget->setCurrentIndex(1);
}
