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

#include "connectioneditordialog.h"

#include <QPushButton>
#include <QVBoxLayout>

#include <KLocalizedString>

ConnectionEditorDialog::ConnectionEditorDialog(const NetworkManager::ConnectionSettings::Ptr &connection,
                                               QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_buttonBox(new QDialogButtonBox(this))
    , m_connectionEditorTabWidget(new ConnectionEditorTabWidget(connection, parent, f))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_connectionEditorTabWidget);

    m_buttonBox->setStandardButtons(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    m_buttonBox->button(QDialogButtonBox::Save)->setEnabled(m_connectionEditorTabWidget->isValid());
    layout->addWidget(m_buttonBox);

    setLayout(layout);

    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &ConnectionEditorDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &ConnectionEditorDialog::reject);
    connect(m_connectionEditorTabWidget, &ConnectionEditorTabWidget::validityChanged, this, &ConnectionEditorDialog::onValidityChanged);

    if (connection->id().isEmpty()) {
        setWindowTitle(i18n("New Connection (%1)", connection->typeAsString(connection->connectionType())));
    } else {
        setWindowTitle(i18n("Edit Connection '%1'", connection->id()));
    }
}

ConnectionEditorDialog::~ConnectionEditorDialog()
{
}

NMVariantMapMap ConnectionEditorDialog::setting() const
{
    return m_connectionEditorTabWidget->setting();
}

void ConnectionEditorDialog::onValidityChanged(bool valid)
{
    m_buttonBox->button(QDialogButtonBox::Save)->setEnabled(valid);
}

