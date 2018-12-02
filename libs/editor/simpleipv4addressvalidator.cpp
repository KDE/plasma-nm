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

#include <QStringList>
#include <QVector>

SimpleIpV4AddressValidator::SimpleIpV4AddressValidator(QObject *parent, AddressStyle style)
    : QValidator(parent)
    , m_addressStyle(style)
{
    switch (style) {
        case Base:
            m_validator.setRegularExpression(QRegularExpression(QLatin1String("[0-9, ]{1,3}\\.[0-9, ]{1,3}\\.[0-9, ]{1,3}\\.[0-9, ]{1,3}")));
            break;
        case WithCidr:
            m_validator.setRegularExpression(QRegularExpression(QLatin1String("([0-9]{1,3}\\.){3,3}[0-9]{1,3}/[0-9]{1,2}")));
            break;
        case WithPort:
            m_validator.setRegularExpression(QRegularExpression(QLatin1String("([0-9]{1,3}\\.){3,3}[0-9]{1,3}:[0-9]{1,5}")));
            break;
    }
}

SimpleIpV4AddressValidator::~SimpleIpV4AddressValidator()
{
}

QValidator::State SimpleIpV4AddressValidator::validate(QString &address, int &pos) const
{
    QValidator::State maskResult = checkWithInputMask(address, pos);
    if (QValidator::Invalid == maskResult) {
        return QValidator::Invalid;
    }

    // this list will be filled with tetrad values. It can be used to make
    // some additional correctness checks on the last validation step.
    QList<int> tetrads;

    QValidator::State tetradResult = checkTetradsRanges(address, tetrads);
    if (QValidator::Invalid == tetradResult)
        return QValidator::Invalid;
    else if (QValidator::Intermediate == tetradResult || QValidator::Intermediate == maskResult)
        return QValidator::Intermediate;
    else
        return QValidator::Acceptable;
}

QValidator::State SimpleIpV4AddressValidator::checkWithInputMask(QString &value, int &pos) const
{
    return m_validator.validate(value, pos);
}

QValidator::State SimpleIpV4AddressValidator::checkTetradsRanges(QString &value, QList<int> &tetrads) const
{
    QStringList temp;
    QVector<QStringRef> addrParts;
    QStringList cidrParts;
    QStringList portParts;

    switch (m_addressStyle) {
    case Base:
        addrParts = value.splitRef(QLatin1Char('.'));
        break;

    case WithCidr:
        cidrParts = value.split(QLatin1Char('/'));
        addrParts = cidrParts[0].splitRef(QLatin1Char('.'));
        break;

    case WithPort:
        portParts = value.split(QLatin1Char(':'));
        addrParts = portParts[0].splitRef(QLatin1Char('.'));
        break;
    }

    int i = 0;
    // fill in the list with invalid values
    tetrads << -1 << -1 << -1 << -1;

    // lets check address parts
    Q_FOREACH (const QStringRef &part, addrParts) {
        if (part.isEmpty()) {
            if (i != (addrParts.size() - 1)) {
                // qCDebug(PLASMA_NM) << "part.isEmpty()";
                return QValidator::Invalid;
            }
            // the last tetrad can be empty, continue...
            return QValidator::Intermediate;
        }

        tetrads[i] = part.toInt();

        if (tetrads[i] > 255) {
            // qCDebug(PLASMA_NM) << "tetrads[i] > 255";
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
        // qCDebug(PLASMA_NM) << "QValidator::Intermediate";
        return QValidator::Intermediate;
    }
    else {
        if (m_addressStyle == WithCidr) {
            if (cidrParts.size() > 1) {
                value += QLatin1String("/");
                if (!cidrParts[1].isEmpty()) {
                    int cidrValue = cidrParts[1].toInt();
                    if (cidrValue > 32) {
                        return QValidator::Invalid;
                    } else {
                        value += cidrParts[1];
                        return QValidator::Acceptable;
                    }
                }
                else {
                    return QValidator::Intermediate;
                }
            }
        } else if (m_addressStyle == WithPort) {
            if (portParts.size() > 1) {
                value += QLatin1String(":");
                if (!portParts[1].isEmpty()) {
                    int portValue = portParts[1].toInt();
                    if (portValue > 65535) {
                        return QValidator::Invalid;
                    } else {
                        value += portParts[1];
                        return QValidator::Acceptable;
                    }
                }
                else {
                    return QValidator::Intermediate;
                }
            }
        }

        // qCDebug(PLASMA_NM) << "QValidator::Acceptable";
        return QValidator::Acceptable;
    }
}
