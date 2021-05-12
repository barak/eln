// Data/TableBlockData.H - This file is part of NotedELN

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

// TableBlockData.H

#ifndef TABLEBLOCKDATA_H

#define TABLEBLOCKDATA_H

#include "TextBlockData.h"

class TableBlockData: public TextBlockData {
  Q_OBJECT;
public:
  TableBlockData(Data *parent=0);
  virtual ~TableBlockData();
  class TableData const *table() const;
  virtual bool isEmpty() const;
  class TableData *table();
  TextBlockData *deepCopyAsTextBlock() const;
  static TableBlockData *deepCopyFromTextBlock(TextBlockData *, int pos);
protected:
  void loadMore(QVariantMap const &src);
private:
  class TableData *table_;
};

#endif
