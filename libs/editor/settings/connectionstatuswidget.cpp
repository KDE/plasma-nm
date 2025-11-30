/*
    SPDX-FileCopyrightText: 2025 Alexander Wilms <f.alexander.wilms@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "connectionstatuswidget.h"
#include "connectiondetails.h"

#include <KLocalizedString>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMetaObject>
#include <QSizePolicy>
#include <QVBoxLayout>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Device>

ConnectionStatusWidget::ConnectionStatusWidget(const QString &connectionUuid, QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_connectionUuid(connectionUuid)
{
    auto *mainLayout = new QVBoxLayout(this);

    // Create horizontal layout to center the form
    auto *hLayout = new QHBoxLayout();
    hLayout->addStretch(1);

    // Create QFormLayout for connection details
    m_detailsLayout = new QFormLayout();
    m_detailsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    m_detailsLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_detailsLayout->setLabelAlignment(Qt::AlignRight);
    hLayout->addLayout(m_detailsLayout);

    hLayout->addStretch(1);

    mainLayout->addLayout(hLayout);
    mainLayout->addStretch(1);

    updateConnectionDetails();
}

ConnectionStatusWidget::~ConnectionStatusWidget() = default;

void ConnectionStatusWidget::setConnectionUuid(const QString &uuid)
{
    if (m_connectionUuid != uuid) {
        m_connectionUuid = uuid;
        updateConnectionDetails();
    }
}

void ConnectionStatusWidget::setDetailsSource(QObject *networkModelItem)
{
    m_detailsSource = networkModelItem;
    m_connection = nullptr;
    m_device = nullptr;
    updateConnectionDetails();
}

void ConnectionStatusWidget::setConnectionAndDevice(const NetworkManager::Connection::Ptr &connection, const NetworkManager::Device::Ptr &device)
{
    m_connection = connection;
    m_device = device;
    m_detailsSource = nullptr;
    updateConnectionDetails();
}

QVariantList ConnectionStatusWidget::getConnectionDetails() const
{
    // If a details source was set (NetworkModelItem from applet), call its details() method
    if (m_detailsSource) {
        QVariantList result;
        // Call: QVariantList details() const
        bool success = QMetaObject::invokeMethod(
            m_detailsSource,
            "details",
            Qt::DirectConnection,
            Q_RETURN_ARG(QVariantList, result)
        );

        if (success) {
            return result;
        }
    }

    // If connection and device were set directly (KCM), use ConnectionDetails helper
    if (m_connection && m_device) {
        return ConnectionDetails::getConnectionDetails(m_connection, m_device);
    }

    // Fallback: no details source or connection/device
    QVariantList details;
    details << QVariantMap{{QStringLiteral("label"), i18n("Status")}, {QStringLiteral("value"), i18n("Activate this connection to view details")}};
    return details;
}

void ConnectionStatusWidget::updateConnectionDetails()
{
    // Clear existing form rows
    while (m_detailsLayout->rowCount() > 0) {
        m_detailsLayout->removeRow(0);
    }

    QVariantList details = getConnectionDetails();

    if (details.isEmpty()) {
        return;
    }

    // Add each detail as a form row
    for (const QVariant &item : details) {
        QVariantMap map = item.toMap();
        QString label = map.value(QStringLiteral("label")).toString();
        QString value = map.value(QStringLiteral("value")).toString();

        // Check if this is a section header
        if (label == QLatin1String("__section__")) {
            // Add some spacing before the section (except for first section)
            if (m_detailsLayout->rowCount() > 0) {
                m_detailsLayout->addItem(new QSpacerItem(0, 12, QSizePolicy::Minimum, QSizePolicy::Fixed));
            }

            QLabel *sectionLabel = new QLabel(value, this);
            QFont sectionFont = sectionLabel->font();
            sectionFont.setBold(true);
            sectionFont.setPointSize(sectionFont.pointSize() + 1);
            sectionLabel->setFont(sectionFont);
            sectionLabel->setAlignment(Qt::AlignHCenter);

            // Add as two-column spanning row
            m_detailsLayout->addRow(sectionLabel);

            // Add small spacing after section header
            m_detailsLayout->addItem(new QSpacerItem(0, 4, QSizePolicy::Minimum, QSizePolicy::Fixed));
            continue;
        }

        // Check if this is a spacer (empty label and value)
        if (label.isEmpty() && value.isEmpty()) {
            // Add vertical spacing
            m_detailsLayout->addItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
            continue;
        }

        // Create value label widget
        QLabel *valueLabel = new QLabel(value, this);
        valueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

        // Add row with label text and value widget
        m_detailsLayout->addRow(label + QLatin1Char(':'), valueLabel);
    }
}

#include "moc_connectionstatuswidget.cpp"
