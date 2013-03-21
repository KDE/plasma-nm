/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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

#include "qmlplugins.h"

#include <QtDeclarative/QDeclarativeItem>

#include "handler.h"
#include "modelitem.h"
#include "monitor.h"
#include "model.h"
#include "sortmodel.h"
#include "appletinfo.h"
#include "secretagent.h"

void QmlPlugins::registerTypes(const char* uri)
{
    qmlRegisterType<Handler>(uri, 0, 1, "Handler");
    qmlRegisterType<Model>(uri, 0, 1, "Model");
    qmlRegisterType<SortModel>(uri, 0, 1, "SortModel");
    qmlRegisterType<AppletInfo>(uri, 0, 1, "AppletInfo");
    qmlRegisterType<SecretAgent>(uri, 0, 1, "SecretAgent");
}

Q_EXPORT_PLUGIN2(plasmanm, QmlPlugins)
