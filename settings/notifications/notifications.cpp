/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "notifications.h"

#include <QHBoxLayout>

#include <KAboutData>
#include <KLocale>
#include <KPluginFactory>
#include <KNotifyConfigWidget>


K_PLUGIN_FACTORY(NotificationsWidgetFactory, registerPlugin<NotificationsWidget>();)
K_EXPORT_PLUGIN(NotificationsWidgetFactory("plasma_networkmanagement_notifications", "plasma_applet_org.kde.plasma-networkmanagement"))

NotificationsWidget::NotificationsWidget(QWidget *parent, const QVariantList &args)
    : KCModule(NotificationsWidgetFactory::componentData(), parent, args),
      m_notifyWidget(new KNotifyConfigWidget(this))
{
    QHBoxLayout * layout = new QHBoxLayout(this);
    m_notifyWidget->setApplication("plasma-networkmanagement"); // name of the .notifyrc file
    layout->addWidget(m_notifyWidget);
    setLayout(layout);

    setButtons(KCModule::Apply); // apply calls the save() method
}

NotificationsWidget::~NotificationsWidget()
{
}

void NotificationsWidget::save()
{
    m_notifyWidget->save();
}
