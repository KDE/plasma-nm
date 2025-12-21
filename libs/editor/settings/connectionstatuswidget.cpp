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
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QVariant>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Device>

ConnectionStatusWidget::ConnectionStatusWidget(const QString &connectionUuid, QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_connectionUuid(connectionUuid)
{
    m_stackedLayout = new QStackedLayout(this);
    m_stackedLayout->setSizeConstraint(QLayout::SetMinimumSize);
    m_stackedLayout->setContentsMargins(0, 0, 0, 0);

    // Page 0: Disconnected state
    auto *disconnectedPage = new QWidget(this);
    auto *disconnectedLayout = new QVBoxLayout(disconnectedPage);
    m_disconnectedLabel = new QLabel(i18n("Disconnected"), this);
    disconnectedLayout->addStretch(1);
    disconnectedLayout->addWidget(m_disconnectedLabel, 0, Qt::AlignHCenter);
    disconnectedLayout->addStretch(1);
    m_stackedLayout->addWidget(disconnectedPage);

    // Page 1: Details state
    auto *detailsPage = new QWidget(this);
    // Create horizontal layout to center the form
    auto *detailsPageLayout = new QHBoxLayout(detailsPage);
    detailsPageLayout->addStretch(1);

    m_formContainer = new QWidget(this);
    m_formContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_containerLayout = new QVBoxLayout(m_formContainer);
    m_containerLayout->setContentsMargins(0, 0, 0, 0);

    // Create QFormLayout for connection details
    m_detailsLayout = new QFormLayout();
    m_detailsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    m_detailsLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_detailsLayout->setLabelAlignment(Qt::AlignRight);
    m_containerLayout->addLayout(m_detailsLayout);
    m_containerLayout->setAlignment(Qt::AlignTop);

    detailsPageLayout->addWidget(m_formContainer);
    detailsPageLayout->addStretch(1);

    m_stackedLayout->addWidget(detailsPage);

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

void ConnectionStatusWidget::setConnectionAndDevice(const NetworkManager::Connection::Ptr &connection,
                                                    const NetworkManager::Device::Ptr &device,
                                                    const QString &accessPointPath)
{
    m_connection = connection;
    m_device = device;
    m_accessPointPath = accessPointPath;
    m_detailsSource = nullptr;
    updateConnectionDetails();
}

QList<ConnectionDetails::ConnectionDetailSection> ConnectionStatusWidget::getConnectionDetails() const
{
    // If a details source was set (NetworkModelItem from applet), call its detailsList() method
    if (m_detailsSource) {
        QVariant result;
        bool success = QMetaObject::invokeMethod(m_detailsSource, "detailsList", Qt::DirectConnection, Q_RETURN_ARG(QVariant, result));
        if (success && result.canConvert<QList<ConnectionDetails::ConnectionDetailSection>>()) {
            return result.value<QList<ConnectionDetails::ConnectionDetailSection>>();
        }
    }

    // If connection and device were set directly (KCM), use ConnectionDetails helper
    if (m_device) {
        return ConnectionDetails::getConnectionDetails(m_connection, m_device, m_accessPointPath);
    }

    // Fallback: no details source or connection/device
    return {};
}

void ConnectionStatusWidget::updateConnectionDetails()
{
    // Clear existing form rows before populating
    while (m_detailsLayout->rowCount() > 0) {
        m_detailsLayout->removeRow(0);
    }

    const auto detailsList = getConnectionDetails();

    if (detailsList.isEmpty()) {
        m_stackedLayout->setCurrentIndex(0); // Show "Disconnected" label
    } else {
        bool firstSection = true;
        // Populate the details form
        for (const auto &section : detailsList) {
            const QString &sectionName = section.title;
            const auto &items = section.details;

            // Add some spacing before the section (except for first section)
            if (!firstSection) {
                m_detailsLayout->addItem(new QSpacerItem(0, 12, QSizePolicy::Minimum, QSizePolicy::Fixed));
            }
            firstSection = false;

            QLabel *sectionLabel = new QLabel(sectionName, this);
            QFont sectionFont = sectionLabel->font();
            sectionFont.setBold(true);
            sectionFont.setPointSize(sectionFont.pointSize() + 1);
            sectionLabel->setFont(sectionFont);
            sectionLabel->setAlignment(Qt::AlignHCenter);
            // Add as two-column spanning row
            m_detailsLayout->addRow(sectionLabel);
            // Add small spacing after section header
            m_detailsLayout->addItem(new QSpacerItem(0, 4, QSizePolicy::Minimum, QSizePolicy::Fixed));

            for (const auto &[label, value] : items) {
                // Create value label widget
                QLabel *valueLabel = new QLabel(value, this);
                valueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
                // Add row with label text and value widget
                m_detailsLayout->addRow(label + QLatin1Char(':'), valueLabel);
            }
        }
        m_stackedLayout->setCurrentIndex(1); // Show details form
    }
}