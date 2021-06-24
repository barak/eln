// Items/Item.H - This file is part of NotedELN

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

// Item.H

#ifndef ITEM_H

#define ITEM_H

#include "Data.h"
#include <QGraphicsObject>
#include <QObject>
#include <QPointer>
#include <Qt>
#include "Style.h"
#include "ElnAssert.h"

#define DATAACCESS(T) T const *data() const { return constCastData<T>(); } \
                      T *data() { return castData<T>(); }

class Item: public QGraphicsObject {
  Q_OBJECT;
public:
  Item(Data *d, Item *parent);
  virtual ~Item();
  virtual void deleteLater();
  bool beingDeleted() const { return d==0; }
  DATAACCESS(Data);
  Item *parent() const;
  class Mode *mode() const;
  virtual QRectF netBounds() const;
  virtual bool excludeFromNet() const;
  bool isWritable() const;
  virtual void makeWritable();
  virtual void makeWritableNoRecurse();
  Style const &style() const;
  QVariant style(QString) const;
  virtual class BlockItem const *ancestralBlock() const;
  virtual class BlockItem *ancestralBlock();
  QList<Item*> allChildren() const;
  template<class T> QList<T*> children() const {
    QList<T *> res;
    foreach (QGraphicsItem *c0, QGraphicsObject::childItems()) {
      T *c = dynamic_cast<T*>(c0);
      if (c)
 	res.append(c);
    }
    return res;
  }
  template<class T> T *firstChild() const {
    foreach (QGraphicsItem *c0, QGraphicsObject::childItems()) {
      T *c = dynamic_cast<T*>(c0);
      if (c)
	return c;
    }
    return 0;
  }
  virtual void setScale(qreal);
  virtual void modeChangeUnderCursor(); // only called for item under mouse
  Item *findDescendant(QString uuid); // 0 if not found
public:
  static Item *create(Data *d, Item *parent=0);
  class GfxNoteItem *createGfxNote(QPointF p0); // create by dragging
  GfxNoteItem *newGfxNote(QPointF p0) { return newGfxNote(p0, p0); }
  virtual GfxNoteItem *newGfxNote(QPointF p0, QPointF p1);
  virtual bool makesOwnNotes() const { return false; }
  Item *glowItem() const;
  virtual void waitForLoadComplete();
public: // but only for use in derived class source files
  template <class DT, class IT> class Creator {
  public:
    Creator<DT, IT>(QString typ) {
      Item::creators()[typ] = &create;
    }
    static Item *create(Data *d, Item *parent=0) {
      return new IT(dynamic_cast<DT*>(d), parent);
    }
  };
public:
  static Qt::CursorShape defaultCursorShape();
  virtual bool changesCursorShape() const;
  virtual Qt::CursorShape cursorShape(Qt::KeyboardModifiers m) const;
protected:
  virtual void keyPressEvent(QKeyEvent *);
  virtual void keyReleaseEvent(QKeyEvent *);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
  template<class T> T *castData() {
    T *d1 = dynamic_cast<T *>(d);
    ASSERT(d1);
    return d1;
  }
  template<class T> T const *constCastData() const {
    T const *d1 = dynamic_cast<T const*>(d);
    ASSERT(d1);
    return d1;
  }
private:
  static QMap<QString, Item *(*)(Data *, Item *)> &creators();
private: // these must never be called
  Item(Item const &); // copy constructor
  QGraphicsObject *parentObject() const; // hide from QGraphicsObject
  QGraphicsItem *parentItem() const; // hide from QGraphicsObject
  QList<QGraphicsItem *> childItems() const; // hide from QGraphicsItem
  const QObjectList &children() const; // hide from QObject
  // QObject's parent() has been overridden
  void setParent(QObject *); // hide from QObject
protected: // but don't use unless you know what you're doing
  void perhapsCreateGlow(Qt::KeyboardModifiers m);
  void removeGlow();
  bool shouldGlow() const;
  void setGlowItem(Item *child);
  void unsetGlowItem(Item *child);
private:
  Data *d;
  bool writable;
  QPointer<Item> glowitem;
};

#endif
