/*
Copyright 2009 Paul Marchouk <pmarchouk@gmail.com>

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

#include "simpleipv4addressvalidator.h"

#include <KDebug>
#include <QStringList>

SimpleIpV4AddressValidator::SimpleIpV4AddressValidator(QObject *parent)
    : QValidator(parent)
{
}

SimpleIpV4AddressValidator::~SimpleIpV4AddressValidator()
{
}

QValidator::State SimpleIpV4AddressValidator::validate(QString &address, int &pos) const
{
    if (QValidator::Invalid == checkWithInputMask(address, pos)) {
        return QValidator::Invalid;
    }

    // this list will be filled with tetrad values. It can be used to make
    // some additional correctness checks on the last validation step.
    QList<int> tetrads;

    return checkTetradsRanges(address, tetrads);
}

QValidator::State SimpleIpV4AddressValidator::checkWithInputMask(QString &value, int &pos) const
{
    QRegExpValidator v(QRegExp("[0-9, ]{1,3}\\.[0-9, ]{1,3}\\.[0-9, ]{1,3}\\.[0-9, ]{1,3}"), 0);

    return v.validate(value, pos);
}

QValidator::State SimpleIpV4AddressValidator::checkTetradsRanges(QString &value, QList<int> &tetrads) const
{
    QStringList temp;
    QStringList addrParts = value.split(QLatin1Char('.'));
    int i = 0;
    // fill in the list with invalid values
    tetrads << -1 << -1 << -1 << -1;

    // lets check address parts
    foreach(const QString &part, addrParts) {
        if (part.isEmpty()) {
            if (i != (addrParts.size() - 1)) {
                //kDebug() << "part.isEmpty()";
                return QValidator::Invalid;
            }
            // the last tetrad can be empty, continue...
            return QValidator::Intermediate;
        }

        tetrads[i] = part.toInt();

        if (tetrads[i] > 255) {
            //kDebug() << "tetrads[i] > 255";
            return QValidator::Invalid;
        }

        // correct tetrad value: for example, 001 -> 1
        temp.append(QString::number(tetrads[i]));

        i++;
    }

    // replace input string with the corrected version
    value = temp.join(QLatin1String("."));

    if (i < 4) {
        // not all tetrads are filled... continue
        //kDebug() << "QValidator::Intermediate";
        return QValidator::Intermediate;
    }
    else {
        //kDebug() << "QValidator::Acceptable";
        return QValidator::Acceptable;
    }
}
