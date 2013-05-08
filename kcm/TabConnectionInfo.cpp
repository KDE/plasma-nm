#include "TabConnectionInfo.h"
#include "ui_TabConnectionInfo.h"

TabConnectionInfo::TabConnectionInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabConnectionInfo)
{
    ui->setupUi(this);
}

TabConnectionInfo::~TabConnectionInfo()
{
    delete ui;
}

void TabConnectionInfo::setConnection(const NetworkManager::Settings::Connection::Ptr &connection)
{
    if (connection) {
//        NetworkManager::Settings::ConnectionSettings::Ptr settings
//        ui->lastUsedL->setText(connection->);
    }
}
