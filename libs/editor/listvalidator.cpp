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

#include "listvalidator.h"

#include <QStringList>

ListValidator::ListValidator(QObject *parent)
    : QValidator(parent)
    , inner(nullptr)
{
}

ListValidator::~ListValidator()
{
}

QValidator::State ListValidator::validate(QString &text, int &pos) const
{
    Q_ASSERT(inner);
    Q_UNUSED(pos);

    QStringList strings = text.split(QLatin1Char(','));
    int unusedPos;
    QValidator::State state = Acceptable;
    for (QStringList::iterator i = strings.begin(); i != strings.end(); ++i) {
        QString string = i->trimmed();
        const int position = i->indexOf(string);
        const int size = string.size();
        const QValidator::State current = inner->validate(string, unusedPos);
        i->replace(position, size, string);
        if (current == Invalid) {
            state = Invalid;
            break;
        }
        if (current == Intermediate) {
            if (state == Intermediate) {
                state = Invalid;
                break;
            }
            state = Intermediate;
        }
    }
    text = strings.join(QLatin1Char(','));
    return state;
}

void ListValidator::setInnerValidator(QValidator *validator)
{
    inner = validator;
}
