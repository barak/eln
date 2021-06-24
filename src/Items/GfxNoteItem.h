// Items/GfxNoteItem.H - This file is part of NotedELN

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

// GfxNoteItem.H

#ifndef GFXNOTEITEM_H

#define GFXNOTEITEM_H

#include "Item.h"
#include <QGraphicsObject>
#include "GfxNoteData.h"

class GfxNoteItem: public Item {
  Q_OBJECT;  
public:
  GfxNoteItem(GfxNoteData *data, Item *parent=0);
  virtual ~GfxNoteItem();
  DATAACCESS(GfxNoteData);
  QRectF boundingRect() const;
  QRectF netBoundingRect() const;
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  static GfxNoteItem *newNote(QPointF p0, QPointF p1, Item *parent);
  virtual void makeWritable();
  void childMousePress(QPointF pscene, Qt::MouseButton, Qt::KeyboardModifiers);
  void lineMousePress(QPointF pscene, Qt::MouseButton, Qt::KeyboardModifiers);
  class TextItem *textItem() { return text; }
  virtual void setScale(qreal f);
  void translate(QPointF dxy);
  void setFocus();
  bool shouldResize(QPointF pscene) const;
protected:
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
protected:
  Qt::CursorShape cursorShape(Qt::KeyboardModifiers) const;
  bool changesCursorShape() const;
private slots:
  void updateTextPos(); // must be called after scale change
  void futileMovementKey(int, Qt::KeyboardModifiers);
  void abandon();
private:
  QPointF nearestCorner(QPointF pbase=QPointF(), bool *inside_return = 0);
private:
  friend class LineItem;
  void perhapsCreateGlow(Qt::KeyboardModifiers m);
  void removeGlow();
protected:
  class TextItem *text;
  class QGraphicsLineItem *line;
  bool resizing;
  double initialTextWidth;
};

#endif
