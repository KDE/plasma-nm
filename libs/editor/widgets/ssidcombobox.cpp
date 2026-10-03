/*
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "ssidcombobox.h"
#include "uiutils.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessDevice>

#include <KLocalizedString>

#include <QApplication>
#include <QPainter>
#include <QStyledItemDelegate>

namespace
{

// Paints each dropdown row as two lines: the SSID at full size, and (when
// present) a dimmer signal/security/frequency line underneath it.
class SsidItemDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        const QString details = index.data(SsidComboBox::NetworkDetailsRole).toString();
        if (details.isEmpty()) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        const QWidget *widget = opt.widget;
        QStyle *style = widget ? widget->style() : QApplication::style();

        painter->save();

        // Highlight fills the whole row.
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, widget);

        const QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, widget);
        if (!opt.icon.isNull())
            opt.icon.paint(painter, iconRect, Qt::AlignVCenter | Qt::AlignHCenter);

        const QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);

        const QFontMetrics ssidMetrics(opt.font);
        QFont detailFont = opt.font;
        detailFont.setPointSizeF(qMax(detailFont.pointSizeF() * 0.85, 7.0));
        const QFontMetrics detailMetrics(detailFont);

        constexpr int kLineGap = 2; // gap between the two lines
        const int blockHeight = ssidMetrics.height() + kLineGap + detailMetrics.height();

        // Center the two-line block inside the row -> even padding above and below.
        const int top = textRect.top() + (textRect.height() - blockHeight) / 2;

        const QRect ssidRect(textRect.left(), top, textRect.width(), ssidMetrics.height());
        const QRect detailRect(textRect.left(), ssidRect.bottom() + kLineGap, textRect.width(), detailMetrics.height());

        const QColor textColor = opt.palette.color(opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text);
        QColor detailColor = textColor;
        detailColor.setAlphaF(0.65);

        painter->setFont(opt.font);
        painter->setPen(textColor);
        painter->drawText(ssidRect, Qt::AlignLeft | Qt::AlignVCenter, ssidMetrics.elidedText(opt.text, Qt::ElideRight, ssidRect.width()));

        painter->setFont(detailFont);
        painter->setPen(detailColor);
        painter->drawText(detailRect, Qt::AlignLeft | Qt::AlignVCenter, detailMetrics.elidedText(details, Qt::ElideRight, detailRect.width()));

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        const QString details = index.data(SsidComboBox::NetworkDetailsRole).toString();
        if (details.isEmpty())
            return QStyledItemDelegate::sizeHint(option, index);

        QFont detailFont = option.font;
        detailFont.setPointSizeF(qMax(detailFont.pointSizeF() * 0.85, 7.0));

        constexpr int kLineGap = 2;
        constexpr int kVerticalPadding = 3; // padding above + below the text block

        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(QFontMetrics(option.font).height() + kLineGap + QFontMetrics(detailFont).height() + 2 * kVerticalPadding);
        return size;
    }
};

}

bool signalCompare(const NetworkManager::WirelessNetwork::Ptr &one, const NetworkManager::WirelessNetwork::Ptr &two)
{
    return one->signalStrength() > two->signalStrength();
}

SsidComboBox::SsidComboBox(QWidget *parent)
    : KComboBox(parent)
{
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    setItemDelegate(new SsidItemDelegate(this));

    connect(this, &SsidComboBox::editTextChanged, this, &SsidComboBox::ssidChanged);
    connect(this, QOverload<int>::of(&SsidComboBox::activated), this, &SsidComboBox::slotCurrentIndexChanged);
}

QString SsidComboBox::ssid() const
{
    // The edit field always holds the SSID itself, whether picked or typed.
    return currentText();
}

void SsidComboBox::slotCurrentIndexChanged(int index)
{
    // Only fires on a deliberate pick (connected to activated), not on typing.
    // The security stored on the item travels with the selection, so it works
    // even if the network has since dropped out of range.
    const QString ssid = itemData(index).toString();
    setEditText(ssid);

    const QVariant security = itemData(index, SsidComboBox::NetworkSecurityRole);
    const NetworkManager::WirelessSecurityType securityType =
        security.isValid() ? static_cast<NetworkManager::WirelessSecurityType>(security.toInt()) : NetworkManager::UnknownSecurity;
    Q_EMIT networkSelected(ssid, securityType);
}

void SsidComboBox::init(const QString &ssid, NetworkManager::WirelessSecurityType savedSecurity)
{
    m_initialSsid = ssid;

    // qCDebug(PLASMA_NM_EDITOR_LOG) << "Initial ssid:" << m_initialSsid;

    QList<NetworkManager::WirelessNetwork::Ptr> networks;

    for (const NetworkManager::Device::Ptr &device : NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();

            for (const NetworkManager::WirelessNetwork::Ptr &newNetwork : wifiDevice->networks()) {
                bool found = false;
                for (const NetworkManager::WirelessNetwork::Ptr &existingNetwork : networks) {
                    if (newNetwork->ssid() == existingNetwork->ssid()) {
                        if (newNetwork->signalStrength() > existingNetwork->signalStrength()) {
                            networks.removeOne(existingNetwork);
                            break;
                        } else {
                            found = true;
                            break;
                        }
                    }
                }
                if (!found) {
                    networks << newNetwork;
                }
            }
        }
    }

    std::sort(networks.begin(), networks.end(), signalCompare);
    addSsidsToCombo(networks);

    int index = findData(m_initialSsid);
    if (index == -1) {
        // Saved network that isn't currently visible: insert it so it stays
        // selectable and label it from the saved security. An empty SSID is a
        // new connection, so leave it as a plain editable entry.
        const bool secure = savedSecurity != NetworkManager::UnknownSecurity && savedSecurity != NetworkManager::NoneSecurity;

        if (m_initialSsid.isEmpty()) {
            insertItem(0, m_initialSsid, m_initialSsid);
        } else {
            insertItem(0, QIcon::fromTheme(secure ? QStringLiteral("object-locked") : QStringLiteral("object-unlocked")), m_initialSsid, m_initialSsid);

            const QString details = savedSecurity != NetworkManager::UnknownSecurity
                ? i18nc("@item:inlistbox saved wireless network not currently in range, %1 is the security type",
                        "Saved, %1",
                        UiUtils::labelFromWirelessSecurity(savedSecurity))
                : i18nc("@item:inlistbox saved wireless network not currently in range", "Saved");
            setItemData(0, details, SsidComboBox::NetworkDetailsRole);
            setItemData(0, details, Qt::ToolTipRole);
            // Keep the saved security on the item so re-selecting it doesn't reset to None.
            if (savedSecurity != NetworkManager::UnknownSecurity) {
                setItemData(0, static_cast<int>(savedSecurity), SsidComboBox::NetworkSecurityRole);
            }
        }
        setCurrentIndex(0);
    } else {
        setCurrentIndex(index);
    }
    setEditText(m_initialSsid);
}

void SsidComboBox::addSsidsToCombo(const QList<NetworkManager::WirelessNetwork::Ptr> &networks)
{
    QList<NetworkManager::WirelessDevice::Ptr> wifiDevices;

    for (const NetworkManager::Device::Ptr &dev : NetworkManager::networkInterfaces()) {
        if (dev->type() == NetworkManager::Device::Wifi) {
            wifiDevices << dev.objectCast<NetworkManager::WirelessDevice>();
        }
    }

    for (const NetworkManager::WirelessNetwork::Ptr &network : networks) {
        NetworkManager::AccessPoint::Ptr accessPoint = network->referenceAccessPoint();
        if (!accessPoint) {
            continue;
        }

        for (const NetworkManager::WirelessDevice::Ptr &wifiDev : std::as_const(wifiDevices)) {
            if (wifiDev->findNetwork(network->ssid()) == network) {
                NetworkManager::WirelessSecurityType security =
                    NetworkManager::findBestWirelessSecurity(wifiDev->wirelessCapabilities(),
                                                             true,
                                                             (wifiDev->mode() == NetworkManager::WirelessDevice::Adhoc),
                                                             accessPoint->capabilities(),
                                                             accessPoint->wpaFlags(),
                                                             accessPoint->rsnFlags());

                const bool secure = security != NetworkManager::UnknownSecurity && security != NetworkManager::NoneSecurity;
                addItem(QIcon::fromTheme(secure ? QStringLiteral("object-locked") : QStringLiteral("object-unlocked")),
                        accessPoint->ssid(),
                        accessPoint->ssid());

                const QString details = secure
                    ? i18n("%1%, %2, %3 MHz", network->signalStrength(), UiUtils::labelFromWirelessSecurity(security), accessPoint->frequency())
                    : i18n("%1%, Insecure, %2 MHz", network->signalStrength(), accessPoint->frequency());
                setItemData(count() - 1, details, SsidComboBox::NetworkDetailsRole);
                setItemData(count() - 1, details, Qt::ToolTipRole);
                setItemData(count() - 1, static_cast<int>(security), SsidComboBox::NetworkSecurityRole);
            }
        }
    }
}

#include "moc_ssidcombobox.cpp"
