// App/OneLink.cpp - This file is part of NotedELN

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


#include "OneLink.h"
#include "TextItem.h"
#include "PreviewPopper.h"
#include "ResManager.h"
#include "ElnAssert.h"
#include "SheetScene.h"
#include "PageView.h"
#include "TextData.h"
#include "EventView.h"

#include <QDesktopServices>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QDebug>
#include <QProcess>
#include <QMenu>

OneLink::OneLink(class MarkupData *md, class TextItem *item, bool tryload):
  QObject(item), md(md), ti(item) {
  //  start = end = -1;
  popper = 0;
  busy = false;
  lastRef = "";
  if (tryload)
    update();
}

OneLink::~OneLink() {
}

void OneLink::update() {
  if (ti->isWritable()) {
    Resource *res = resource();
    if (!res || res->needsArchive() || res->needsPreview()) {
      getArchiveAndPreview();
    }
  }
  /*
    This must happen whenever the MarkupData changes or periodically.
    Previously, this was assured because TextMarkings got an update() whenever
    text was inserted or removed from the item. It also had newMark() and
    deleteMark() functions to update the list of HoverRegions. This
    functionality should now move to LinkHelper.
  */
}
  
bool OneLink::mousePress(QGraphicsSceneMouseEvent *e) {
  switch (e->button()) {
  case Qt::LeftButton:
    if (ti->mode()->mode()==Mode::Browse
	|| (e->modifiers() & Qt::ControlModifier)) {
      activate(e->modifiers());
      return true;
    } else {
      return false;
    }
  case Qt::RightButton:
    contextMenu(e);
    return true;
  default:
    return false;
  }
}

void OneLink::contextMenu(QGraphicsSceneMouseEvent *e) {
  Resource *r = resource();
  if (!r)
    return;
  if (r->sourceURL().scheme() == "page") {
    QMenu menu;
    QAction *go = menu.addAction("Go to page");
    QAction *nw = menu.addAction("Open page in new window");
    menu.move(e->screenPos() - QPoint(32, 32));
    QAction *res = menu.exec();
    if (res==go) 
      openPage(false);
    else if (res==nw)
      openPage(true);
  } else {
    QMenu menu;
    QAction *arch = menu.addAction("Open archived copy of resource");
    QAction *orig = menu.addAction("Open original location of resource");
    menu.move(e->screenPos() - QPoint(32, 32));
    QAction *res = menu.exec();
    if (res==arch)
      openArchive();
    else if (res==orig)
      openLink();
  }
}

void OneLink::activate(Qt::KeyboardModifiers m) {
  if (m & Qt::ShiftModifier)
    openLink();
  else 
    openArchive();
}

bool OneLink::mouseDoubleClick(QGraphicsSceneMouseEvent *e) {
  activate(e->modifiers());
  return true;
}

void OneLink::enter(QGraphicsSceneHoverEvent *) {
  if (popper) {
    popper->popup();
  } else {
    QString txt = refText();
    if (txt.isEmpty())
      return;
    Resource *r = resource();
    if (r) {
      popper = new PreviewPopper(r, QRect(), this);
      connect(popper, SIGNAL(clicked(Qt::KeyboardModifiers)),
	      SLOT(activate(Qt::KeyboardModifiers)));
    }
  }
}

void OneLink::leave() {
  if (popper) 
    popper->closeSoon();
}

Resource *OneLink::resource() const {
  ResManager *resmgr = md->resManager();
  if (!resmgr) {
    qDebug() << "OneLink: no resource manager";
    return 0;
  }
  return resmgr->byTag(refText());
}

QString OneLink::refText() const {
  TextCursor c(ti->document(), md->start(), md->end());
  return c.selectedText();
}

void OneLink::openLink() {
  Resource *r = resource();
  if (!r) {
    qDebug() << "OneLink: openURL" << refText() <<  "(no url)";
    return;
  }
  if (r->sourceURL().scheme() == "page") {
    openPage(true);
  } else {
    qDebug() << "OneLink::openLink" << r->sourceURL();
    bool ok = QDesktopServices::openUrl(r->sourceURL());
    if (!ok)
      qDebug() << "Failed to open external url" << r->sourceURL();
  }
}

void OneLink::openPage(bool newView) {
  Resource *r = resource();
  ASSERT(r);
  QString tag = r->tag();
  QString path = r->sourceURL().path();

  ASSERT(ti);
  SheetScene *s = dynamic_cast<SheetScene *>(ti->scene());
  ASSERT(s);
  PageView *pv = EventView::eventView();
  if (!pv) {
    qDebug() << "No event view in OneLink::openPage";
    return;
  }

  if (newView)
    pv->newView()->gotoEntryPage(tag, path);
  else
    pv->gotoEntryPage(tag, path);
}

void OneLink::openArchive() {
  Resource *r = resource();
  if (!r) {
    qDebug() << "OneLink: openArchive" << refText() << "(no arch)";
    return;
  }
  
  if (r->sourceURL().scheme() == "page") {
    openPage();
  } else if (r->hasArchive()) {
    QUrl url(QUrl::fromLocalFile((r->archivePath())));
    qDebug() << "OneLink::openArchive" << url;
    bool ok = QDesktopServices::openUrl(url);
    if (!ok)
      qDebug() << "Failed to start external location " << r->archivePath();
  } else {
    openLink();
  }
}  

void OneLink::getArchiveAndPreview() {
  if (refText()==lastRef || busy)
    return; // we know we can't do it

  ResManager *resmgr = md->resManager();
  if (!resmgr) {
    qDebug() << "OneLink: no resource manager";
    return;
  }
  
  QString newRef = refText();
  if (newRef!=lastRef && !lastRef.isEmpty()) {
    md->detachResource(lastRef);
    md->resManager()->perhapsDropResource(lastRef);
  }
  
  lastRef = newRef;

  Resource *r = resmgr->byTag(newRef);
  if (r) {
    lastRefIsNew = false;
  } else {
    r = md->resManager()->newResource(newRef);
    lastRefIsNew = true;
  }
  connect(r, SIGNAL(finished()), SLOT(downloadFinished()),
	  Qt::UniqueConnection);

  if (!md->resourceTags().contains(newRef))
    md->attachResource(newRef);
  
  busy = true;
  r->getArchiveAndPreview();
}

void OneLink::downloadFinished() {
  if (!busy) {
    qDebug() << "not busy";
    return;
  }
  ASSERT(busy);
  ResManager *resmgr = md->resManager();
  Resource *r = resmgr->byTag(lastRef);
  if (!r || refText()!=lastRef) {
    /* Either the resource got destroyed somehow, or we have already
       changed; so we're not interested in the results anymore. */
    if (lastRefIsNew) 
      resmgr->dropResource(r);
    /* This is actually dangerous in case two refs are looking for the
       same resource simultaneously and one gets edited. */
  } else if (!r->needsArchive() || !r->needsPreview()
             || !r->title().isEmpty() || !r->description().isEmpty()) {
    // at least somewhat successful
    md->attachResource(lastRef);
  } else {
    // utter failure
    if (lastRefIsNew) {
      resmgr->dropResource(r);
    }
  }
  busy = false;
}
