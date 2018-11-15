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

#include "wireguardkeyvalidator.h"

WireGuardKeyValidator::WireGuardKeyValidator(QObject *parent)
    : QValidator(parent)
{
    m_validator = new QRegularExpressionValidator(this);
    // A WireGuard key is Base64 encoded and in human readable form consists
    // of 43 Alpha-numeric or  '+' or '/' with a 44th character of an equal sign.
    m_validator->setRegularExpression(QRegularExpression(QLatin1String("[0-9a-zA-Z\\+/]{43,43}=")));
}

WireGuardKeyValidator::~WireGuardKeyValidator()
{
}

QValidator::State WireGuardKeyValidator::validate(QString &address, int &pos) const
{
    return m_validator->validate(address, pos);
}
