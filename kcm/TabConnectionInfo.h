#ifndef TABCONNECTIONINFO_H
#define TABCONNECTIONINFO_H

#include <QWidget>

#include <NetworkManagerQt/Connection>

namespace Ui {
class TabConnectionInfo;
}

class TabConnectionInfo : public QWidget
{
    Q_OBJECT
public:
    explicit TabConnectionInfo(QWidget *parent = 0);
    ~TabConnectionInfo();

    void setConnection(const NetworkManager::Connection::Ptr &connection);
    
private:
    Ui::TabConnectionInfo *ui;
};

#endif // TABCONNECTIONINFO_H
