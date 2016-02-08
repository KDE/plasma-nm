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
// #include <KConfigGroup>
#include <KPluginFactory>
#include <KSharedConfig>
// #include <KDecoration2/DecorationButton>
// #include <KNewStuff3/KNS3/DownloadDialog>
#include <kdeclarative/kdeclarative.h>
// Qt
// #include <QDBusConnection>
// #include <QDBusMessage>
// #include <QFontDatabase>
#include <QMenu>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>
// #include <QSortFilterProxyModel>
// #include <QStandardPaths>
#include <QVBoxLayout>

K_PLUGIN_FACTORY(KCMNetworkConfigurationFactory, registerPlugin<KCMNetworkmanagement>();)

KCMNetworkmanagement::KCMNetworkmanagement(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
    , m_ui(new Ui::KCMForm)
{
    QWidget * mainWidget = new QWidget(this);
    m_ui->setupUi(mainWidget);

    m_quickView = new QQuickView(0);
    KDeclarative::KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(m_quickView->engine());
    kdeclarative.setTranslationDomain(QStringLiteral(TRANSLATION_DOMAIN));
    kdeclarative.setupBindings();

    QWidget *widget = QWidget::createWindowContainer(m_quickView, this);
    QVBoxLayout* layout = new QVBoxLayout(m_ui->view);
    layout->addWidget(widget);

    m_quickView->setResizeMode(QQuickView::SizeRootObjectToView);
    m_quickView->setSource(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kcm_networkmanagement/qml/main.qml"))));

    QVBoxLayout *l = new QVBoxLayout(this);
    l->addWidget(mainWidget);
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
    KCModule::load();
}

void KCMNetworkmanagement::save()
{
    KCModule::save();
}

#include "kcm.moc"

