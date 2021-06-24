// Items/GfxImageItem.cpp - This file is part of NotedELN

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

// GfxImageItem.C

#include <QEventLoop>
#include "GfxImageItem.h"
#include "GfxImageData.h"
#include "GfxVideoData.h"
#include "EntryScene.h"
#include "Mode.h"
#include "GfxNoteData.h"
#include "GfxNoteItem.h"
#include "GfxMarkItem.h"
#include "GfxSketchItem.h"
#include "GfxLineItem.h"
#include "BlockItem.h"
#include "Cursors.h"
#include <QDesktopServices>
#include "Notebook.h"
#include "ImageLoader.h"

#include <QProcess>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QKeyEvent>
#include "ResManager.h"
#include <QCursor>
#include <math.h>

static Item::Creator<GfxImageData, GfxImageItem> c("gfximage");

GfxImageItem::GfxImageItem(GfxImageData *data, Item *parent):
  Item(data, parent) {
  loading = false;
  pixmap = new QGraphicsPixmapItem(this);
  pixmap->setAcceptedMouseButtons(0);

  dragType = None;
  cropallowed = true;
  
  // get the image, crop it, etc.
  ResManager *resmgr = data->resManager();
  if (!resmgr) {
    qDebug() << "GfxImageItem: no resource manager";
    return;
  }
  Resource *res = resmgr->byTag(data->resName());
  if (!res) {
    qDebug() << "GfxImageItem: missing resource" << data->resName();
    return;
  }
  if (dynamic_cast<GfxVideoData*>(data)) {
    pixmap->setPixmap(QPixmap(QSize(data->width(), data->height())));
    // this place holder is important for sizing during block construction
  } else {
    constexpr int SIZETHRESHOLD = 100000;
    if (data->width() * data->height() > SIZETHRESHOLD) {
      ImageLoader *ldr = new ImageLoader;
      loading = true;
      connect(ldr, &ImageLoader::loaded,
              [this](QImage img) {
                this->loading = false;
                this->setImage(img);
                emit loadComplete();
              });
      image = QImage(data->width(), data->height(), QImage::Format_RGB32);
      image.fill(QColor(255, 255, 200)); // pale yellow during loading
      ldr->loadThenDelete(res->archivePath());
    } else {
      if (!image.load(res->archivePath())) {
        qDebug() << "GfxImageItem: image load failed for " << data->resName()
                 << res->archivePath();
        image = QImage(data->width(), data->height(), QImage::Format_RGB32);
        image.fill(QColor(255, 50, 50)); // pink-grey for failed to load
      }
    }
    pixmap->setPixmap(QPixmap::fromImage(image.copy(data->cropRect().toRect())));
  }
  setScale(data->scale());
  setPos(data->pos());
  pixmap->setPos(data->cropRect().topLeft());
  pixmap->setTransformationMode(Qt::SmoothTransformation);

  foreach (GfxData *gd, data->children<GfxData>()) {
    Item *i = create(gd, this);
    i->setScale(1./data->scale());
  }
}

GfxImageItem::~GfxImageItem() {
}

QPointF GfxImageItem::moveDelta(QGraphicsSceneMouseEvent *e) {
  QPointF delta = mapToParent(e->pos()) - dragStart;
  return delta;
}

static double euclideanLength(QPointF a) {
  return sqrt(a.x()*a.x() + a.y()*a.y());
}

void GfxImageItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  QPointF ppos = mapToParent(e->pos());
  switch (dragType) {
  case None:
    break;
  case Move:
    setPos(data()->pos() + moveDelta(e));
    break;
  case ResizeTopLeft: {
    QPointF xy0 = cropStart.bottomRight();
    double diag0 = euclideanLength(dragStart - xy0);
    double diag1 = euclideanLength(ppos - xy0);
    setScale(data()->scale() * diag1/diag0);
    setPos(pos() + xy0 - mapToParent(imageBoundingRect().bottomRight()));
  } break;
  case ResizeTopRight: {
    QPointF xy0 = cropStart.bottomLeft();
    double diag0 = euclideanLength(dragStart - xy0);
    double diag1 = euclideanLength(ppos - xy0);
    setScale(data()->scale() * diag1/diag0);
    setPos(pos() + xy0 - mapToParent(imageBoundingRect().bottomLeft()));
  } break;
  case ResizeBottomLeft: {
    QPointF xy0 = cropStart.topRight();
    double diag0 = euclideanLength(dragStart - xy0);
    double diag1 = euclideanLength(ppos - xy0);
    setScale(data()->scale() * diag1/diag0);
    setPos(pos() + xy0 - mapToParent(imageBoundingRect().topRight()));
  } break;
  case ResizeBottomRight: {
    QPointF xy0 = cropStart.topLeft();
    double diag0 = euclideanLength(dragStart - xy0);
    double diag1 = euclideanLength(ppos - xy0);
    setScale(data()->scale() * diag1/diag0);
    setPos(pos() + xy0 - mapToParent(imageBoundingRect().topLeft()));
  } break;
  case CropLeft: {
    double dx = moveDelta(e).x();
    double x0 = cropStart.left();
    double x = x0 + dx;
    if (x < imStart.left())
      x = imStart.left();
    else if (x > cropStart.right())
      x = cropStart.right();
    dx = x - x0;
    QRectF cr = cropStart; cr.setLeft(x);
    dragCrop = mapRectFromParent(cr).toRect();
    pixmap->setPixmap(QPixmap::fromImage(image.copy(dragCrop)));
    pixmap->setPos(dragCrop.topLeft());
  } break;
  case CropRight: {
    double dx = moveDelta(e).x();
    double x0 = cropStart.right();
    double x = x0 + dx;
    if (x < cropStart.left())
      x = cropStart.left();
    else if (x > imStart.right())
      x = imStart.right();
    dx = x - x0;
    QRectF cr = cropStart; cr.setRight(x);
    dragCrop = mapRectFromParent(cr).toRect();
    pixmap->setPixmap(QPixmap::fromImage(image.copy(dragCrop)));
  } break;
  case CropTop: {
    double dy = moveDelta(e).y();
    double y0 = cropStart.top();
    double y = y0 + dy;
    if (y < imStart.top())
      y = imStart.top();
    else if (y > cropStart.bottom())
      y = cropStart.bottom();
    dy = y - y0;
    QRectF cr = cropStart; cr.setTop(y);
    dragCrop = mapRectFromParent(cr).toRect();
    pixmap->setPixmap(QPixmap::fromImage(image.copy(dragCrop)));
    pixmap->setPos(dragCrop.topLeft());
  } break;
  case CropBottom: {
    double dy = moveDelta(e).y();
    double y0 = cropStart.bottom();
    double y = y0 + dy;
    if (y < cropStart.top())
      y = cropStart.top();
    else if (y > imStart.bottom())
      y = imStart.bottom();
    dy = y - y0;
    QRectF cr = cropStart; cr.setBottom(y);
    dragCrop = mapRectFromParent(cr).toRect();
    pixmap->setPixmap(QPixmap::fromImage(image.copy(dragCrop)));
  } break;
  }
}

void GfxImageItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) {
  Resource *r = data()->resManager()->byTag(data()->resName());
  if (!r) {
    qDebug() << "GfxImageItem: double click: no resource";
  } else {
    QUrl url = (e->modifiers() & Qt::ShiftModifier)
      ? r->sourceURL()
      : QUrl::fromLocalFile(r->archivePath());
    bool ok = QDesktopServices::openUrl(url);
    if (!ok)
      qDebug() << "GfxImageItem: Failed to open location " << url;
  }
  e->accept();
}

GfxNoteItem *GfxImageItem::newGfxNote(QPointF p0, QPointF p1) {
  GfxNoteItem *gni = Item::newGfxNote(p0, p1);
  gni->setScale(1./data()->scale());
  return gni;
}

void GfxImageItem::startDrag(QGraphicsSceneMouseEvent *e) {
  dragType = dragTypeForPoint(e->pos());
  dragStart = mapToParent(e->pos());
  cropStart = mapRectToParent(data()->cropRect());
  imStart = mapRectToParent(QRectF(QPointF(0,0), data()->size()));
  dragCrop = data()->cropRect().toRect();
}  

void GfxImageItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  bool take = false;
  if (isWritable()) {
    if (e->modifiers() & Qt::ControlModifier) {
      startDrag(e);
      take = true;
    } else {
      switch (mode()->mode()) {
      case Mode::MoveResize:
        startDrag(e);
        take = true;
        break;
      case Mode::Type:
        createGfxNote(e->pos());
        take = true;
        break;
      case Mode::Mark: {
        GfxMarkItem *mi = GfxMarkItem::newMark(e->pos(), this);
        mi->setScale(1./data()->scale());
        take = true;
      } break;
      case Mode::Draw: 
	if (mode()->drawMode()==Mode::Straightline) {
	  GfxLineItem *li = GfxLineItem::newLine(e->pos(), this);
	  li->setScale(1./data()->scale());
	  li->build(e);
	} else {
	  GfxSketchItem *ski = GfxSketchItem::newSketch(e->pos(), this);
	  ski->setScale(1./data()->scale());
	  ski->build();
	}
        take = true;
	break;
      default:
        break;
      }
    }
  }
  
  if (take) 
    e->accept();
  else 
    e->ignore();
}

void GfxImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  switch (dragType) {
  case None:
    break;
  case Move: 
    data()->setPos(pos());
    break;
  case ResizeTopLeft: case ResizeTopRight:
  case ResizeBottomLeft: case ResizeBottomRight: 
    data()->setPos(pos());
    data()->setScale(scale());
    break;
  case CropLeft: case CropRight:
  case CropTop: case CropBottom:
    data()->setCropRect(dragCrop); // round to integers
    break;
  }
  if (ancestralBlock()) 
    ancestralBlock()->sizeToFit();
  e->accept();
}

GfxImageItem::DragType GfxImageItem::dragTypeForPoint(QPointF p) const {
  p -= data()->cropRect().topLeft();
  double x = p.x();
  double y = p.y();
  double w = imageBoundingRect().width();
  double h = imageBoundingRect().height();
  if (x/w < .25 && y/h < .25)
    return ResizeTopLeft;
  else if (x/w < .25 && y/h > .75)
    return ResizeBottomLeft;
  else if (x/w > .75 && y/h < .25)
    return ResizeTopRight;
  else if (x/w > .75 && y/h > .75)
    return ResizeBottomRight;
  else if (x/w < .15 && cropallowed)
    return CropLeft;
  else if (x/w > .85 && cropallowed)
    return CropRight;
  else if (y/h < .15 && cropallowed)
    return CropTop;
  else if (y/h > .85 && cropallowed)
    return CropBottom;
  else
    return Move;
}

Qt::CursorShape GfxImageItem::cursorForDragType(GfxImageItem::DragType dt) {
  switch (dt) {
  case None:
    return Qt::ArrowCursor;
  case Move:
    return Qt::SizeAllCursor;
  case ResizeTopLeft: case ResizeBottomRight:
    return Qt::SizeFDiagCursor;
  case ResizeTopRight: case ResizeBottomLeft:
    return Qt::SizeBDiagCursor;
  case CropLeft: case CropRight:
    return Qt::SplitHCursor;
  case CropTop: case CropBottom:
    return Qt::SplitVCursor;
  }
  return defaultCursorShape(); // this statement will not be reached
}

bool GfxImageItem::changesCursorShape() const {
  return true;
}

Qt::CursorShape GfxImageItem::cursorShape(Qt::KeyboardModifiers m) const {
  if (mode()->mode()==Mode::MoveResize
      || (m & Qt::ControlModifier))
    return cursorForDragType(dragTypeForPoint(cursorPos));
  else 
    return Qt::CrossCursor;
}

void GfxImageItem::hoverMoveEvent(QGraphicsSceneHoverEvent *e) {
  cursorPos = e->pos(); // cache for the use of modifierChanged
  if (mode()->mode()==Mode::MoveResize
    || (e->modifiers() & Qt::ControlModifier))
    setCursor(cursorForDragType(dragTypeForPoint(cursorPos)));
  else
    setCursor(Qt::CrossCursor);
  e->accept();
}

QRectF GfxImageItem::imageBoundingRect() const {
  return pixmap->mapRectToParent(pixmap->boundingRect());
}

QRectF GfxImageItem::boundingRect() const {
  return imageBoundingRect();
}

void GfxImageItem::makeWritable() {
  Item::makeWritable();
  setAcceptHoverEvents(true);
}

void GfxImageItem::setScale(double s) {
  QGraphicsObject::setScale(s);
  foreach (Item *i, allChildren())
    i->setScale(1./s);
}

void GfxImageItem::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {
}

void GfxImageItem::setImage(QImage img) {
  image = img;
  QImage crop(image.copy(data()->cropRect().toRect()));
  pixmap->setPixmap(QPixmap::fromImage(crop));
}

void GfxImageItem::setCropAllowed(bool a) {
  cropallowed = a;
}

void GfxImageItem::waitForLoadComplete() {
  Item::waitForLoadComplete();
  if (!loading)
    return;
  QEventLoop el;
  connect(this, &GfxImageItem::loadComplete,
          &el, &QEventLoop::quit);
  el.exec(QEventLoop::ExcludeUserInputEvents);
}

  
