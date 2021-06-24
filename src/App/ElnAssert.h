// App/Elnassert.H - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// Elnassert.H

#ifndef ELNASSERT_H

#define ELNASSERT_H

#include <QString>
#include <QDebug>

class AssertedException {
};

void assertion_register_notebook(class Notebook *);

void assertion_crash(QString msg, char const *file, int line);
inline void assertion_noop() { }
void assertion_complain(QString msg, char const *file, int line);

#define ASSERT(cond) ((!(cond)) ? assertion_crash(#cond, __FILE__, __LINE__) : assertion_noop())

#define COMPLAIN(msg) (assertion_complain(msg, __FILE__, __LINE__))
#define MILDASSERT(cond) ((!(cond)) ? assertion_complain(#cond, __FILE__, __LINE__) : assertion_noop())

#endif
