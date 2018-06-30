/*
    Copyright 2009 Andrey Batyiev <batyiev@gmail.com>
    Copyright 2015 Jan Grulich <jgrulich@redhat.com>

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

#ifndef PLASMA_NM_LIST_VALIDATOR_H
#define PLASMA_NM_LIST_VALIDATOR_H

#include <QValidator>

/**
 * This class validates each string item with a validator.
 * String items are separated by comma.
 * The validator should be set with setInnerValidator(..) method.
 * Please note, space characters are allowed only after comma characters.
 */
class ListValidator : public QValidator
{
public:
    ListValidator(QObject *parent);
    ~ListValidator() override;

    State validate(QString &text, int &pos) const override;
    void setInnerValidator(QValidator *validator);

private:
    QValidator *inner;
};

#endif // PLASMA_NM_LIST_VALIDATOR_H

