// Items/GfxNoteItem.cpp - This file is part of NotedELN

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

// GfxNoteItem.C

#include "GfxNoteItem.h"
#include "GfxNoteData.h"
#include "TextItem.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include "ElnAssert.h"
#include "BlockItem.h"
#include <QTextBlock>
#include <QTextLayout>
#include <QTextLine>
#include <math.h>
#include "Cursors.h"

#define MINLINELENGTH 0.1

static Item::Creator<GfxNoteData, GfxNoteItem> c("gfxnote");

//////////////////////////////////////////////////////////////////////
class LineItem: public QGraphicsLineItem {
public:
  LineItem(QLineF l, GfxNoteItem *parent): QGraphicsLineItem(l, parent) {
    note = parent;
    setAcceptHoverEvents(true);
  }
  virtual ~LineItem() {
  }
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *e) {
    ASSERT(note);
    note->lineMousePress(e->scenePos(), e->button(), e->modifiers());
  }
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
    ASSERT(note);
    auto m = e->modifiers();
    if (m & Qt::ShiftModifier)
      m |= Qt::ControlModifier;
    note->perhapsCreateGlow(e->modifiers());
  }
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
    ASSERT(note);
    note->removeGlow();
  }
private:
  GfxNoteItem *note;
};

GfxNoteItem::GfxNoteItem(GfxNoteData *data, Item *parent):
  Item(data, parent) {
  setPos(data->pos());
  line = 0;
  text = new TextItem(data->text(), this);
  connect(text, &TextItem::futileMovementKey,
	  this, &GfxNoteItem::futileMovementKey);
  text->setFont(style().font("note-font"));
  text->setDefaultTextColor(QColor(style().string("note-text-color")));
  if (data->textWidth()>1)
    text->setTextWidth(data->textWidth(), false);

  text->setLineHeight(style().lineSpacing("note-font",
					  "note-line-spacing"));
  
  connect(text, &TextItem::abandoned,
	  this, &GfxNoteItem::abandon, Qt::QueuedConnection);
		     
  setFlag(ItemIsFocusable);
  connect(text->document(), &TextItemDoc::contentsChanged,
	  this, &GfxNoteItem::updateTextPos);
  if (data->text()->lineStarts().isEmpty()) 
    text->document()->relayout();
  else
    text->document()->recalculateCharacterWidths();
  updateTextPos();
}

GfxNoteItem::~GfxNoteItem() {
}

void GfxNoteItem::abandon() {
  if (beingDeleted()) {
    qDebug() << "(GfxNoteItem::abandon: being deleted)";
    return; 
  }
  Item *p = parent();
  if (p && p->beingDeleted()) {
    qDebug() << "(GfxNoteItem::abandon: parent being deleted)";
    return; 
  }
  qDebug() << "GfxNoteItem::abandon";
  qDebug() << "  I am " << this << beingDeleted();
  qDebug() << "  my parent" << parent();
  qDebug() << "  my data" << data();
  qDebug() << "  my data's parent" << data()->parent();

  if (data()->parent())
    data()->parent()->deleteChild(data());

  deleteLater();

  BlockItem *ancestor = ancestralBlock();
  if (ancestor)
    ancestor->sizeToFit();
}

static double sigmoid(double xl, double xr) {
  double w = xr - xl; // width of box
  double dw = w>20 ? 12 : 3*w/5;
  xl += dw/2;
  w -= dw;
  xl += .1*w;
  w *= .8;
  double x = -xl; // posn of point relative to left of box
  if (x<0)
    return xl;
  x = x * .6;
  if (x>w)
    return xl+w;
  //double a = x/w;
  //constexpr double ALPHA = 2;
  //a = pow(a, ALPHA);
  return xl + x; //+ w*a;
}

QPointF GfxNoteItem::nearestCorner(QPointF pbase, bool *insidep) {
  QRectF docr = text->document()->tightBoundingRect()
    .translated(text->pos() - pbase);

  double dx = style().real("note-x-inset") / 2;
  double dx2 = dx;
  double x0, y0;
  double ygood = docr.top()>0 ? docr.top()
    : docr.bottom()<0 ? -docr.bottom()
    : 0;
  double xgood = docr.left()>0 ? docr.left()
    : docr.right()<0 ? - docr.right()
    : 0;
  if (insidep)
    *insidep = false;

  if (ygood>xgood) {
    // coming mostly from above or below
    if (docr.top() > dx) {
      // coming substantially from above
      y0 = docr.top() - dx2;
      x0 = sigmoid(docr.left(), docr.right());
    } else if (docr.bottom() < -dx) {
      // coming substantially from below
      y0 = docr.bottom() + dx2;
      x0 = sigmoid(docr.left(), docr.right());
    } else if (docr.left() > dx) {
      // coming substantially from the left
      x0 = docr.left() - dx2;
      y0 = sigmoid(docr.top(), docr.bottom());
    } else if (docr.right() < -dx) {
      // coming substantially from the right
      x0 = docr.right() + dx2;
      y0 = sigmoid(docr.top(), docr.bottom());
    } else {
      // too close by; what can we do that is reasonable?
      x0 = docr.left();
      y0 = docr.top();
      if (insidep)
	*insidep = true;
    }
  } else {
    // coming mostly from left or right
    if (docr.left() > dx) {
      // coming substantially from the left
      x0 = docr.left() - dx2;
      y0 = sigmoid(docr.top(), docr.bottom());
    } else if (docr.right() < -dx) {
      // coming substantially from the right
      x0 = docr.right() + dx2;
      y0 = sigmoid(docr.top(), docr.bottom());
    } else if (docr.top() > dx) {
      // coming substantially from above
      y0 = docr.top() - dx2;
      x0 = sigmoid(docr.left(), docr.right());
    } else if (docr.bottom() < -dx) {
      // coming substantially from below
      y0 = docr.bottom() + dx2;
      x0 = sigmoid(docr.left(), docr.right());
    } else {
      // too close by; what can we do that is reasonable?
      x0 = docr.left();
      y0 = docr.top();
      if (insidep)
	*insidep = true;
    }
  }
  return pbase + QPointF(x0, y0);
}
  

void GfxNoteItem::updateTextPos() {
  // Position text at delta (with note offset for compatibility)
  QPointF p = data()->delta();
  double yof = style().real("note-y-offset");
  p += QPointF(0, yof);
  text->setPos(p);

  // Auto limit text width
  if (data()->textWidth()<1) {
    QRectF sr = text->mapRectToScene(text->netBounds());
    if (sr.right() >= style().real("page-width")
	- style().real("margin-right-over")) {
      double tw = style().real("page-width")
	- style().real("margin-right-over")
	- sr.left();
      if (tw<36)
	tw = 36;
      if (data()->isWritable())
        data()->setTextWidth(tw);
      text->setTextWidth(tw);
    }
  }

  // Arrange line to be shortest
  if (data()->delta().manhattanLength()>MINLINELENGTH) { // minimum line length
    if (!line) {
      line = new LineItem(QLineF(QPointF(0,0), QPointF(1,1)), this);
      line->setPen(QPen(QBrush(QColor(style().string("note-line-color"))),
			style().real("note-line-width")));
    }
    bool inside = false;
    QPointF oth = nearestCorner(QPointF(0,0), &inside);
    line->setLine(QLineF(QPointF(0,0), oth));
    if (inside)
      line->hide();
    else 
      line->show();
  } else {
    if (line)
      line->hide();
  }

  BlockItem *ancestor = ancestralBlock();
  if (ancestor)
    ancestor->sizeToFit();
}

QRectF GfxNoteItem::boundingRect() const {
  return QRectF();
}

void GfxNoteItem::paint(QPainter *,
			const QStyleOptionGraphicsItem *,
			QWidget *) {
}

void GfxNoteItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  if (resizing) {
    QPointF delta = e->pos() - e->buttonDownPos(Qt::LeftButton);
    double w = initialTextWidth + delta.x();
    if (w<30)
      w = 30;
    text->setTextWidth(w);
  } else {
    bool draganchor = e->modifiers() & Qt::ShiftModifier;
    QPointF delta = e->pos() - e->lastPos();
    if (draganchor) {
      if (!line) {
	/* Create a line */
	line = new LineItem(QLineF(QPointF(0,0), QPointF(1,1)), this);
	line->setPen(QPen(QBrush(QColor(style().string("note-line-color"))),
			  style().real("note-line-width")));
      }
      QLineF l = line->line(); // origLine;
      l.setP1(l.p1() + delta);
      line->setLine(l);
    } else {
      text->setPos(text->pos() + delta);
    }
    if (line) {
      QLineF l = line->line(); // origLine;
      bool inside = false;
      QPointF oth = nearestCorner(l.p1(), &inside);
      l.setP2(oth);
      line->setLine(l);
      if ((oth-l.p1()).manhattanLength() < MINLINELENGTH || inside) 
	line->hide();
      else
	line->show();
    }
  }
  e->accept();
}

void GfxNoteItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  //  unlockBounds();
  ungrabMouse();
  if (resizing) {
    data()->setTextWidth(text->textWidth());
    updateTextPos();
  } else {
    QPointF ptext = text->pos() - QPointF(0, style().real("note-y-offset"));
    QPointF p0 = mapToParent(ptext);
    if (line && !line->isVisible()) {
      delete line;
      line = 0;
    }
    if (line) {
      QPointF pbase = line->line().p1();
      p0 = mapToParent(pbase);
      data()->setDelta(ptext - pbase);
    } else {
      data()->setDelta(QPointF(0, 0));
    }
    data()->setPos(p0);
    setPos(p0);
    updateTextPos();
  }
  BlockItem *ancestor = ancestralBlock();
  if (ancestor)
    ancestor->sizeToFit();
  e->accept();
}

GfxNoteItem *GfxNoteItem::newNote(QPointF p0, QPointF p1, Item *parent) {
  ASSERT(parent);
  GfxNoteData *d = new GfxNoteData(parent->data());
  QPointF sp0 = parent->mapToScene(p0);
  QPointF sp1 = parent->mapToScene(p1);
  d->setPos(p0);
  d->setDelta(sp1-sp0);
  d->setTextWidth(0);

  GfxNoteItem *i = new GfxNoteItem(d, parent);
  i->makeWritable();
  i->setFocus();
  return i;
}

void GfxNoteItem::setFocus() {
  text->setFocus();
}



void GfxNoteItem::childMousePress(QPointF p, Qt::MouseButton b,
				  Qt::KeyboardModifiers m) {
  if (!isWritable())
    return;
  if ((mode()->mode()==Mode::MoveResize
      || (m & Qt::ControlModifier)
       || (m & Qt::ShiftModifier))
      && b==Qt::LeftButton) {
    resizing = shouldResize(p);
    if (resizing) {
      initialTextWidth = data()->textWidth();
      if (initialTextWidth<1) {
	initialTextWidth = text->netBounds().width()+2;
	text->setTextWidth(initialTextWidth);
      }
    }
    grabMouse();
  }
}

void GfxNoteItem::lineMousePress(QPointF, Qt::MouseButton b,
				  Qt::KeyboardModifiers m) {
  if (!isWritable())
    return;
  if ((mode()->mode()==Mode::MoveResize
      || (m & Qt::ControlModifier)
       || (m & Qt::ShiftModifier))
      && b==Qt::LeftButton) {
    resizing = false;
    grabMouse();
  }
}

void GfxNoteItem::makeWritable() {
  Item::makeWritable();
  text->setAllowMoves();
}

void GfxNoteItem::setScale(qreal f) {
  Item::setScale(f);
  updateTextPos();
}

void GfxNoteItem::translate(QPointF dxy) {
  data()->setPos(data()->pos() + dxy);
  setPos(pos() + dxy);
}


void GfxNoteItem::futileMovementKey(int k, Qt::KeyboardModifiers) {
  switch (k) {
  case Qt::Key_Left: case Qt::Key_Up: {
    TextCursor c(text->textCursor());
    c.movePosition(TextCursor::Start);
    text->setTextCursor(c);
  } break;
  case Qt::Key_Right: case Qt::Key_Down: {
    TextCursor c(text->textCursor());
    c.movePosition(TextCursor::End);
    text->setTextCursor(c);
  } break;
  default:
    break;
  }
}


void GfxNoteItem::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  Item::hoverEnterEvent(e);
  text->setGraphicsEffect(0); // prevent double green
}


bool GfxNoteItem::shouldResize(QPointF p) const {
  double tw = data()->textWidth();
  if (tw<=0)
    tw = text->netBounds().width();
  bool should = mapFromScene(p).x()-netBounds().left() > .75*tw;
  return should;
}
 
void GfxNoteItem::perhapsCreateGlow(Qt::KeyboardModifiers m) {
  Item::perhapsCreateGlow(m);
}

void GfxNoteItem::removeGlow() {
  Item::removeGlow();
}

bool GfxNoteItem::changesCursorShape() const {
  return true;
}

Qt::CursorShape GfxNoteItem::cursorShape(Qt::KeyboardModifiers m) const {
  Qt::CursorShape cs = defaultCursorShape();
  if (parent())
    cs = parent()->cursorShape(m);
  if (!isWritable())
    return cs;
  if (mode()->mode() == Mode::MoveResize
      || (m & Qt::ControlModifier)
      || (m & Qt::ShiftModifier))
    cs = Qt::SizeAllCursor;
  if (line)
    line->setCursor(Cursors::refined(cs));
  return cs;
}
