// Items/TitleItem.H - This file is part of NotedELN

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

// TitleItem.H

#ifndef TITLEITEM_H

#define TITLEITEM_H

#include <QGraphicsObject>
#include "DefaultingTextItem.h"
#include "TitleData.h"

class TitleItem: public DefaultingTextItem {
  Q_OBJECT;
public:
  TitleItem(TitleData *data, Item *parent=0);
  TitleItem(TitleData *data, int sheetno, class TextItemDoc *altdoc=0, 
	    Item *parent=0);
  virtual ~TitleItem();
  TitleData const *data() const { ASSERT(d); return d; }
  TitleData *data() { ASSERT(d); return d; }
  virtual void deleteLater();
protected:
  virtual void keyPressEvent(QKeyEvent *e);
private:
  void setStyles();
private:
  TitleData *d;
};

#endif
