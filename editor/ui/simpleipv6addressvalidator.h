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

#ifndef SIMPLEIPV6ADDRESSVALIDATOR_H
#define SIMPLEIPV6ADDRESSVALIDATOR_H

#include <QValidator>

class SimpleIpV6AddressValidator : public QValidator
{
public:
    SimpleIpV6AddressValidator(QObject *parent);
    virtual ~SimpleIpV6AddressValidator();

    virtual State validate(QString &, int &) const;

    /** Check input value with a regular expression describing simple input mask.
     */
    QValidator::State checkWithInputMask(QString &, int &) const;
    /** Function split intput string into tetrads and check them for valid values.
     *  In the tetrads are placed into QList. Input string may be changed.
     */
    QValidator::State checkTetradsRanges(QString &) const;
};

#endif // SIMPLEIPV6ADDRESSVALIDATOR_H
