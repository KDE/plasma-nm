/*
    Copyright 2011 Lamarque Souza <lamarque@kde.org>
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

#include "pindialog.h"

#include <QIntValidator>

#include <KDebug>
#include <kwindowsystem.h>

#include <ModemManagerQt/manager.h>

PinDialog::PinDialog(ModemManager::ModemInterface *modem, const Type type, QWidget *parent)
    : KDialog(parent), m_type(type)
{
    if (modem) {
        m_udi = modem->udi();
#ifdef WITH_MODEMMANAGERQT
        m_name = modem->device();
#else
        m_name = modem->masterDevice();
#endif
        foreach (const Solid::Device &d, Solid::Device::allDevices()) {
            if (d.udi().contains(m_name, Qt::CaseInsensitive)) {
                m_name = d.product();
                if (!m_name.startsWith(d.vendor())) {
                    m_name = d.vendor() + ' ' + m_name;
                }
                break;
            }
        }
    }

    QWidget *w = new QWidget();
    ui = new Ui::PinWidget();
    ui->setupUi(w);
    ui->pin->setPasswordMode(true);

    QIntValidator * validator = new QIntValidator(this);
    validator->setRange(1000, 99999999);
    ui->pin->setValidator(validator);
    ui->pin2->setValidator(validator);

    QIntValidator * validator2 = new QIntValidator(this);
    validator2->setRange(10000000, 99999999);
    ui->puk->setValidator(validator2);

    ui->errorMessage->setHidden(true);
    QRect desktop = KGlobalSettings::desktopGeometry(topLevelWidget());
    setMinimumWidth(qMin(1000, qMax(sizeHint().width(), desktop.width() / 4)));

    pixmapLabel = new QLabel(mainWidget());
    pixmapLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->gridLayout->addWidget(pixmapLabel, 0, 0);
    pixmapLabel->setPixmap(KIcon("dialog-password").pixmap(KIconLoader::SizeHuge));

    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    button(KDialog::Ok)->setText(i18nc("As in 'Unlock cell phone with this pin code'", "Unlock"));
    setMainWidget(w);

    if (m_type == PinPuk) {
        setWindowTitle(i18n("SIM PUK unlock required"));
        ui->title->setText(i18n("SIM PUK Unlock Required"));
        ui->prompt->setText(i18n("The mobile broadband device '%1' requires a SIM PUK code before it can be used.", m_name));
        ui->pukLabel->setText(i18n("PUK code:"));
        ui->pinLabel->setText(i18n("New PIN code:"));
        ui->pin2Label->setText(i18n("Re-enter new PIN code:"));
        ui->chkShowPass->setText(i18n("Show PIN/PUK code"));

        ui->puk->setFocus();
        ui->pukLabel->show();
        ui->puk->show();
        ui->pin2Label->show();
        ui->pin2->show();
    } else {
        setWindowTitle(i18n("SIM PIN unlock required"));
        ui->title->setText(i18n("SIM PIN Unlock Required"));
        ui->prompt->setText(i18n("The mobile broadband device '%1' requires a SIM PIN code before it can be used.", m_name));
        ui->pinLabel->setText(i18n("PIN code:"));
        ui->chkShowPass->setText(i18n("Show PIN code"));

        ui->pin->setFocus();
        ui->pukLabel->hide();
        ui->puk->hide();
        ui->pin2Label->hide();
        ui->pin2->hide();
    }

    ui->puk->clear();
    ui->pin->clear();
    ui->pin2->clear();
    ui->puk->setCursorPosition(0);
    ui->pin->setCursorPosition(0);
    ui->pin2->setCursorPosition(0);

    KWindowSystem::setState(winId(), NET::KeepAbove);
    KWindowSystem::activateWindow(winId());

    move((desktop.width() - width()) / 2, (desktop.height() - height()) / 2);
    connect(ui->chkShowPass, SIGNAL(toggled(bool)), this, SLOT(chkShowPassToggled(bool)));
    connect(ModemManager::notifier(), SIGNAL(modemRemoved(QString)), SLOT(modemRemoved(QString)));
}

PinDialog::~PinDialog()
{
    delete ui;
}

void PinDialog::chkShowPassToggled(bool on)
{
    ui->pin->setPasswordMode(!on);
    ui->pin2->setPasswordMode(!on);
    ui->puk->setPasswordMode(!on);

    ui->puk->setCursorPosition(0);
    ui->pin->setCursorPosition(0);
    ui->pin2->setCursorPosition(0);

    if (m_type == PinPuk) {
        ui->puk->setFocus();
    } else {
        ui->pin->setFocus();
    }
}

void PinDialog::modemRemoved(const QString &udi)
{
    if (udi == m_udi) {
        reject();
    }
}

PinDialog::Type PinDialog::type() const
{
    return m_type;
}

QString PinDialog::pin() const
{
    return ui->pin->text();
}

QString PinDialog::pin2() const
{
    return ui->pin2->text();
}

QString PinDialog::puk() const
{
    return ui->puk->text();
}

void PinDialog::showErrorMessage(const PinDialog::ErrorCode error)
{
    QString msg;
    QFont bold = font();
    ui->pinLabel->setFont(bold);
    ui->pin2Label->setFont(bold);
    ui->pukLabel->setFont(bold);
    bold.setBold(true);
    switch(error) {
    case PinCodeTooShort:
        msg = i18n("PIN code too short. It should be at least 4 digits.");
        ui->pin->setFocus();
        ui->pinLabel->setFont(bold);
        break;
    case PinCodesDoNotMatch:
        msg = i18n("The two PIN codes do not match");
        ui->pin2->setFocus();
        ui->pin2Label->setFont(bold);
        break;
    case PukCodeTooShort:
        msg = i18n("PUK code too short. It should be 8 digits.");
        ui->puk->setFocus();
        ui->pukLabel->setFont(bold);
        break;
    default:
        msg = i18n("Unknown Error");
    }
    ui->errorMessage->setText(msg, KTitleWidget::ErrorMessage);
    adjustSize();
}


void PinDialog::accept()
{
    if (m_type == PinPuk) {
        if (pin() != pin2()) {
            showErrorMessage(PinCodesDoNotMatch);
            return;
        } else if (puk().length() < 8) {
            showErrorMessage(PukCodeTooShort);
            return;
        }
    }

    if (pin().length() < 4) {
        showErrorMessage(PinCodeTooShort);
        return;
    }

    QDialog::accept();
}
