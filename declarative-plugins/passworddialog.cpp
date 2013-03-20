#include "passworddialog.h"

PasswordDialog::PasswordDialog(NetworkManager::Settings::Setting *setting, const QStringList &neededSecrets, const QString &ssid, QWidget *parent) :
    QDialog(parent), m_ui(new Ui::PasswordDialog), m_neededSecrets(neededSecrets)
{
    m_ui->setupUi(this);
    m_ui->labelIcon->setPixmap(QIcon::fromTheme("dialog-password").pixmap(32));

    if (neededSecrets.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "list of secrets is empty!!!";
    }

    if (setting->type() == NetworkManager::Settings::Setting::WirelessSecurity || setting->type() == NetworkManager::Settings::Setting::Security8021x) {
        m_ui->labelText->setText(i18n("For accessing the wireless network '%1' you need to provide a password below", ssid));
    } else {
        m_ui->labelText->setText(i18n("Please provide a password below"));
    }
}

PasswordDialog::~PasswordDialog()
{
    delete m_ui;
}

QVariantMap PasswordDialog::secrets() const
{
    QVariantMap result;

    if (!m_ui->password->text().isEmpty())
        result.insert(m_neededSecrets.first(), m_ui->password->text());

    return result;
}
