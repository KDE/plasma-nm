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

#ifndef SIMPLEIPLISTVALIDATOR_H
#define SIMPLEIPLISTVALIDATOR_H

#include <QValidator>
#include "simpleipv4addressvalidator.h"
#include "simpleipv6addressvalidator.h"

class Q_DECL_EXPORT SimpleIpListValidator : public QValidator
{
public:
    enum AddressType {Ipv4, Ipv6, Both};
    enum AddressStyle {Base, WithCidr, WithPort};

    explicit SimpleIpListValidator(AddressStyle style = AddressStyle::Base,
                                   AddressType allow = AddressType::Both, QObject *parent = nullptr);
    ~SimpleIpListValidator() override;

    State validate(QString &, int &) const override;

private:
    SimpleIpV6AddressValidator *m_ipv6Validator;
    SimpleIpV4AddressValidator *m_ipv4Validator;
};

#endif // SIMPLEIPV4ADDRESSVALIDATOR_H
