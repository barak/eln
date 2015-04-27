// Items/TableItem.H - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// TableItem.H

#ifndef TABLEITEM_H

#define TABLEITEM_H

#include "TextItem.h"
#include "TableData.h"
#include "TextCursor.h"
#include <QList>

class TableItem: public TextItem {
  Q_OBJECT;
public:
  class Cell {
  public:
    Cell(TableItem const *tbl, int c=-1, int r=-1): tbl(tbl), c(c), r(r) { }
    int row() const { return r; }
    int column() const { return c; }
    void setRow(int r1) { r = r1; }
    void setColumn(int c1) { c = c1; }
    bool isValid() const { return c>=0 && r>=0; }
    int firstPosition() const;
    int lastPosition() const;
  private:
    TableItem const *tbl;
    int c, r;
  };
public:
  TableItem(TableData *data, Item *parent);
  virtual ~TableItem();
  DATAACCESS(TableData);
  void gotoCell(int row, int col, bool goEnd=false);
  void selectCell(int row, int col);
  void insertRow(int before);
  void insertColumn(int before);
  void deleteRows(int r0, int nr);
  void deleteColumns(int c0, int nc);
  bool isCellEmpty(int r, int c) const;
  bool isColumnEmpty(int c) const;
  bool isRowEmpty(int r) const;
  int lastNonEmptyCellInRow(int r) const; // 0 if none
  QList<TextCursor> normalizeSelection(TextCursor const &) const;
  bool isWholeCellSelected(TextCursor const &) const;
public slots:
  bool normalizeCursorPosition(); // true if changed
protected slots:
  virtual void docChange();
signals:
  void unicellular(TableData *); // emitted when we are left as just one cell
signals: 
  void mustNormalizeCursor(); // private use only: must enforce cursor posn
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void keyPressEvent(QKeyEvent *);
  void focusInEvent(QFocusEvent *);
  bool tryToPaste();
private:
  bool keyPressAsMotion(QKeyEvent *e, Cell const &cell);
  bool keyPressWithControl(QKeyEvent *e);
  Cell cellAt(TextCursor const &) const;
  Cell cellAt(int pos) const;
  Cell cell(int c, int r) const;
private:
  bool ignoreChanges;
  int ctrla_r0, ctrla_c0;
};

#endif
