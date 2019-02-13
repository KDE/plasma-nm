/*
    Copyright 2018 Bruce Anderson <banderson19com@san.rr.com>

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

#include "debug.h"
#include "wireguardwidget.h"
#include "wireguardadvancedwidget.h"
#include "simpleipv4addressvalidator.h"
#include "simpleiplistvalidator.h"
#include "wireguardkeyvalidator.h"

#include <QDBusMetaType>
#include <QPointer>
#include <KColorScheme>

#include "nm-wireguard-service.h"

class WireGuardSettingWidget::Private
{
public:
    Private();

    Ui_WireGuardProp ui;
    NetworkManager::VpnSetting::Ptr setting;
    KSharedConfigPtr config;
    QPalette warningPalette;
    QPalette normalPalette;
    WireGuardKeyValidator *keyValidator;
    bool addressValid;
    bool privateKeyValid;
    bool publicKeyValid;
    bool allowedIpsValid;
    bool endpointValid;
};

WireGuardSettingWidget::Private::Private(void)
    : addressValid(false)
    , privateKeyValid(false)
    , publicKeyValid(false)
    , allowedIpsValid(false)
    , endpointValid(true)    // optional so blank is valid
{
}

WireGuardSettingWidget::WireGuardSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
    : SettingWidget(setting, parent)
    , d(new Private)
{
    qDBusRegisterMetaType<NMStringMap>();

    d->ui.setupUi(this);
    d->setting = setting;

    d->config = KSharedConfig::openConfig();
    d->warningPalette = KColorScheme::createApplicationPalette(d->config);
    d->normalPalette = KColorScheme::createApplicationPalette(d->config);
    KColorScheme::adjustBackground(d->warningPalette, KColorScheme::NegativeBackground, QPalette::Base,
                                   KColorScheme::ColorSet::View, d->config);

    KColorScheme::adjustBackground(d->normalPalette, KColorScheme::NormalBackground, QPalette::Base,
                                   KColorScheme::ColorSet::View, d->config);


    connect(d->ui.addressIPv4LineEdit, &QLineEdit::textChanged, this, &WireGuardSettingWidget::checkAddressValid);
    connect(d->ui.addressIPv6LineEdit, &QLineEdit::textChanged, this, &WireGuardSettingWidget::checkAddressValid);
    connect(d->ui.privateKeyLineEdit, &PasswordField::textChanged, this, &WireGuardSettingWidget::checkPrivateKeyValid);
    connect(d->ui.publicKeyLineEdit, &QLineEdit::textChanged, this, &WireGuardSettingWidget::checkPublicKeyValid);
    connect(d->ui.allowedIPsLineEdit, &QLineEdit::textChanged, this, &WireGuardSettingWidget::checkAllowedIpsValid);
    connect(d->ui.endpointLineEdit, &QLineEdit::textChanged, this, &WireGuardSettingWidget::checkEndpointValid);

    d->ui.privateKeyLineEdit->setPasswordModeEnabled(true);

    connect(d->ui.btnAdvanced, &QPushButton::clicked, this, &WireGuardSettingWidget::showAdvanced);

    SimpleIpV4AddressValidator *ip4WithCidrValidator = 
        new SimpleIpV4AddressValidator(this, SimpleIpV4AddressValidator::AddressStyle::WithCidr);
    d->ui.addressIPv4LineEdit->setValidator(ip4WithCidrValidator);

    // Create a validator for the IPv6 address line edit
    // Address must be a valid IP address with a CIDR suffix
    SimpleIpV6AddressValidator *ip6WithCidrValidator =
        new SimpleIpV6AddressValidator(this, SimpleIpV6AddressValidator::AddressStyle::WithCidr);

    d->ui.addressIPv6LineEdit->setValidator(ip6WithCidrValidator);

    // This is done as a private variable rather than a local variable so it can be
    // used both here and to validate the private key later
    d->keyValidator = new WireGuardKeyValidator(this);
    d->ui.publicKeyLineEdit->setValidator(d->keyValidator);

    // Create validator for Endpoint
    SimpleIpV4AddressValidator *endpointValidator =
        new SimpleIpV4AddressValidator(this, SimpleIpV4AddressValidator::AddressStyle::WithPort);
    d->ui.endpointLineEdit->setValidator(endpointValidator);

    // Create validator for AllowedIPs
    SimpleIpListValidator *allowedIPsValidator = new SimpleIpListValidator(this, SimpleIpListValidator::WithCidr,
                                                                           SimpleIpListValidator::Both);
    d->ui.allowedIPsLineEdit->setValidator(allowedIPsValidator);

    // Connect for setting check
    watchChangedSetting();

    KAcceleratorManager::manage(this);

    if (setting && !setting->isNull()) {
        loadConfig(d->setting);
    }

    // Set the initial backgrounds on all the widgets
    checkAddressValid();
    checkPrivateKeyValid();
    checkPublicKeyValid();
    checkAllowedIpsValid();
    checkEndpointValid();

}

WireGuardSettingWidget::~WireGuardSettingWidget()
{
    delete d;
}

void WireGuardSettingWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_UNUSED(setting)

    // General settings
    const NMStringMap dataMap = d->setting->data();

    d->ui.addressIPv4LineEdit->setText(dataMap[NM_WG_KEY_ADDR_IP4]);
    d->ui.addressIPv6LineEdit->setText(dataMap[NM_WG_KEY_ADDR_IP6]);
    d->ui.privateKeyLineEdit->setText(dataMap[NM_WG_KEY_PRIVATE_KEY]);
    d->ui.publicKeyLineEdit->setText(dataMap[NM_WG_KEY_PUBLIC_KEY]);
    d->ui.allowedIPsLineEdit->setText(dataMap[NM_WG_KEY_ALLOWED_IPS]);
    d->ui.endpointLineEdit->setText(dataMap[NM_WG_KEY_ENDPOINT]);
}

void WireGuardSettingWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    // Currently WireGuard does not have any secrets
    Q_UNUSED(setting)
}

QVariantMap WireGuardSettingWidget::setting() const
{
    NMStringMap data = d->setting->data();
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_WIREGUARD));

    // required settings
    setProperty(data, QLatin1String(NM_WG_KEY_ADDR_IP4), d->ui.addressIPv4LineEdit->displayText());
    setProperty(data, QLatin1String(NM_WG_KEY_ADDR_IP6), d->ui.addressIPv6LineEdit->displayText());
    setProperty(data, QLatin1String(NM_WG_KEY_PRIVATE_KEY), d->ui.privateKeyLineEdit->text());
    setProperty(data, QLatin1String(NM_WG_KEY_PUBLIC_KEY), d->ui.publicKeyLineEdit->displayText());
    setProperty(data, QLatin1String(NM_WG_KEY_ALLOWED_IPS), d->ui.allowedIPsLineEdit->displayText());
    setProperty(data, QLatin1String(NM_WG_KEY_ENDPOINT), d->ui.endpointLineEdit->displayText());

    setting.setData(data);

    return setting.toMap();
}

void WireGuardSettingWidget::setProperty(NMStringMap &data, const QLatin1String &key, const QString &value) const
{
    if (!value.isEmpty())
        data.insert(key, value);
    else
        data.remove(key);
}

void WireGuardSettingWidget::showAdvanced()
{
    QPointer<WireGuardAdvancedWidget> adv = new WireGuardAdvancedWidget(d->setting, this);

    connect(adv.data(), &WireGuardAdvancedWidget::accepted,
            [adv, this] () {
                NetworkManager::VpnSetting::Ptr advData = adv->setting();
                if (!advData.isNull()) {
                    d->setting->setData(advData->data());
                }
            });
    connect(adv.data(), &WireGuardAdvancedWidget::finished,
            [adv] () {
                if (adv) {
                    adv->deleteLater();
                }
            });
    adv->setModal(true);
    adv->show();
}

bool WireGuardSettingWidget::isValid() const
{
    return d->addressValid
           && d->privateKeyValid
           && d->publicKeyValid
           && d->allowedIpsValid
           && d->endpointValid;
}

void WireGuardSettingWidget::checkAddressValid()
{
    int pos = 0;
    QLineEdit *widget = d->ui.addressIPv4LineEdit;
    QString value(widget->displayText());
    bool ip4valid = (widget->validator()->validate(value, pos) == QValidator::Acceptable);
    bool ip4present = !widget->displayText().isEmpty();

    widget = d->ui.addressIPv6LineEdit;
    value = widget->displayText();
    bool ip6valid = QValidator::Acceptable == widget->validator()->validate(value, pos);
    bool ip6present = !widget->displayText().isEmpty();

    d->addressValid = (ip4valid && ip6valid) || (ip4valid && !ip6present) || (!ip4present && ip6valid);

    setBackground(d->ui.addressIPv4LineEdit, d->addressValid);
    setBackground(d->ui.addressIPv6LineEdit, d->addressValid);

    slotWidgetChanged();
}

void WireGuardSettingWidget::checkPrivateKeyValid()
{
    int pos = 0;
    PasswordField *widget = d->ui.privateKeyLineEdit;
    QString value = widget->text();
    d->privateKeyValid = QValidator::Acceptable == d->keyValidator->validate(value, pos);
    setBackground(widget, d->privateKeyValid);
    slotWidgetChanged();
}

void WireGuardSettingWidget::checkPublicKeyValid()
{
    int pos = 0;
    QLineEdit *widget = d->ui.publicKeyLineEdit;
    QString value = widget->displayText();
    d->publicKeyValid = QValidator::Acceptable == widget->validator()->validate(value, pos);
    setBackground(widget, d->publicKeyValid);
    slotWidgetChanged();
}

void WireGuardSettingWidget::checkAllowedIpsValid()
{
    int pos = 0;
    QLineEdit *widget = d->ui.allowedIPsLineEdit;
    QString value = widget->displayText();
    d->allowedIpsValid = QValidator::Acceptable == widget->validator()->validate(value, pos);
    setBackground(widget, d->allowedIpsValid);
    slotWidgetChanged();
}

void WireGuardSettingWidget::checkEndpointValid()
{
    int pos = 0;
    QLineEdit *widget = d->ui.endpointLineEdit;
    QString value = widget->displayText();
    d->endpointValid = QValidator::Acceptable == widget->validator()->validate(value, pos)
                       || value.isEmpty();
    setBackground(widget, d->endpointValid);
    slotWidgetChanged();
}

void WireGuardSettingWidget::setBackground(QWidget *w, bool result) const
{
    if (result)
        w->setPalette(d->normalPalette);
    else
        w->setPalette(d->warningPalette);
}
