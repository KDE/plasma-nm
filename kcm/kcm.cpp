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

#include "kcm.h"

// KDE
#include <KPluginFactory>
#include <KSharedConfig>
#include <kdeclarative/kdeclarative.h>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Settings>

// Qt
#include <QMenu>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>
#include <QVBoxLayout>

K_PLUGIN_FACTORY(KCMNetworkConfigurationFactory, registerPlugin<KCMNetworkmanagement>();)

KCMNetworkmanagement::KCMNetworkmanagement(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
    , m_handler(new Handler(this))
    , m_tabWidget(nullptr)
    , m_ui(new Ui::KCMForm)
    , m_quickView(nullptr)
{
    QWidget *mainWidget = new QWidget(this);
    m_ui->setupUi(mainWidget);

    m_quickView = new QQuickView(0);
    KDeclarative::KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(m_quickView->engine());
    kdeclarative.setTranslationDomain(QStringLiteral(TRANSLATION_DOMAIN));
    kdeclarative.setupBindings();

    QWidget *widget = QWidget::createWindowContainer(m_quickView, this);
    QVBoxLayout *layout = new QVBoxLayout(m_ui->connectionView);
    layout->addWidget(widget);

    m_quickView->setResizeMode(QQuickView::SizeRootObjectToView);
    m_quickView->setSource(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kcm_networkmanagement/qml/main.qml"))));

    QObject *rootItem = m_quickView->rootObject();
    connect(rootItem, SIGNAL(selectedConnectionChanged(QString)), this, SLOT(onSelectedConnectionChanged(QString)));

    QVBoxLayout *l = new QVBoxLayout(this);
    l->addWidget(mainWidget);

    setButtons(Button::Apply);
}

KCMNetworkmanagement::~KCMNetworkmanagement()
{
}

void KCMNetworkmanagement::defaults()
{
    KCModule::defaults();
}

void KCMNetworkmanagement::load()
{
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
    if (connection) {
        NetworkManager::ConnectionSettings::Ptr connectionSettings = connection->settings();
        // Re-load the connection again to load stored values
        if (m_tabWidget) {
            m_tabWidget->setConnection(connectionSettings);
        }
    }
    
    KCModule::load();
}

void KCMNetworkmanagement::save()
{
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
    
    if (connection) {
        m_handler->updateConnection(connection, m_tabWidget->setting());
    }
    
    KCModule::save();
}

void KCMNetworkmanagement::onSelectedConnectionChanged(const QString &connectionPath)
{
    m_currentConnectionPath = connectionPath;
    
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
    if (connection) {
        NetworkManager::ConnectionSettings::Ptr connectionSettings = connection->settings();
        if (m_tabWidget) {
            m_tabWidget->setConnection(connectionSettings);
        } else {
            m_tabWidget = new ConnectionEditorTabWidget(connectionSettings);
            QVBoxLayout *layout = new QVBoxLayout(m_ui->connectionConfiguration);
            layout->addWidget(m_tabWidget);
        }

        Q_EMIT changed(true);
    }
}

#include "kcm.moc"

