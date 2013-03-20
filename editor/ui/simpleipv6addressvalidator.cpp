/*
Copyright 2011 Ilia Kats <ilia-kats@gmx.net>, based on work by Paul Marchouk <pmarchouk@gmail.com>

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

#include "simpleipv6addressvalidator.h"

#include <KDebug>
#include <QStringList>

SimpleIpV6AddressValidator::SimpleIpV6AddressValidator(QObject *parent)
    : QValidator(parent)
{
}

SimpleIpV6AddressValidator::~SimpleIpV6AddressValidator()
{
}

QValidator::State SimpleIpV6AddressValidator::validate(QString &address, int &pos) const
{
    if(QValidator::Invalid == checkWithInputMask(address, pos)) {
        return QValidator::Invalid;
    }

    return checkTetradsRanges(address);
}

QValidator::State SimpleIpV6AddressValidator::checkWithInputMask(QString &value, int &pos) const
{
    QRegExpValidator v(QRegExp("([0-9a-fA-F]{1,4}|:)+"), 0);

    return v.validate(value, pos);
}

QValidator::State SimpleIpV6AddressValidator::checkTetradsRanges(QString &value) const
{
    QStringList addrParts = value.split(QLatin1Char(':'));
    int number = addrParts.size();
    if (number > 8)
    {
        return QValidator::Invalid;
    }

    // lets check address parts
    bool emptypresent = false;
    int i = 1;
    foreach(QString part, addrParts) { // krazy:exclude=foreach
        if (part.isEmpty() && i < number) {
            if (emptypresent) {
                //kDebug() << "part.isEmpty()";
                return QValidator::Invalid;
            }
            else if (!emptypresent)
            {
                part.setNum(0,16);
                emptypresent = true;
            }
        }
        i++;

        bool ok;
        if (part.toInt(&ok, 16) > 65535) {
            return QValidator::Invalid;
        }
    }
    if (number < 8 && !emptypresent)
        return QValidator::Intermediate;

    return QValidator::Acceptable;

}
