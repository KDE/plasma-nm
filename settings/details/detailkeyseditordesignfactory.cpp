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

#include <QtPlugin>

#include <KGlobal>

#include "detailkeyseditor.h"
#include "detailkeyseditordesignfactory.h"

static const KCatalogLoader loader(QLatin1String("plasma_applet_org.kde.plasma-nm"));

DetailKeysEditorDesignerFactory::DetailKeysEditorDesignerFactory(QObject *parent)
: QObject(parent)
{
}

bool DetailKeysEditorDesignerFactory::isContainer() const
{
    return false;
}

QIcon DetailKeysEditorDesignerFactory::icon() const
{
    return QIcon();
}

QString DetailKeysEditorDesignerFactory::group() const
{
    return QString();
}

QString DetailKeysEditorDesignerFactory::includeFile() const
{
    return "detailkeyseditor.h";
}

QString DetailKeysEditorDesignerFactory::name() const
{
    return "DetailKeysEditor";
}

QString DetailKeysEditorDesignerFactory::toolTip() const
{
    return QString();
}

QString DetailKeysEditorDesignerFactory::whatsThis() const
{
    return QString();
}

QWidget * DetailKeysEditorDesignerFactory::createWidget(QWidget *parent)
{
    return new DetailKeysEditor(parent);
}

Q_EXPORT_PLUGIN2(plasmanmwidgets, DetailKeysEditorDesignerFactory)

