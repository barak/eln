// SearchResultScene.cpp

#include "SearchResultScene.H"
#include "TOCEntry.H"
#include <QGraphicsItem>
#include "SearchResItem.H"
#include <QTextDocument>
#include <QTextCursor>
#include "Roman.H"

SearchResultScene::SearchResultScene(QString title, QList<SearchResult> results,
                                     Data *data, QObject *parent):
  BaseScene(data, parent), ttl(title), results(results) {
  book = data->book();
}

SearchResultScene::~SearchResultScene() {
}

Style const &SearchResultScene::style() const {
  return book->style();
}

void SearchResultScene::update(QList<SearchResult> res) {
  results = res;
  populate();
}

void SearchResultScene::populate() {
  BaseScene::populate();
  foreach (TOCItem *i, headers)
    delete i;
  headers.clear();
  sheetnos.clear();

  int oldPage = -1;

  int sheet = 0;
  double y0 = style().real("margin-top");
  double y1 = style().real("page-height") - style().real("margin-bottom");
  double y = y0;
  QGraphicsItem *lastLine = 0;
  foreach (SearchResult const &r, results) {
    if (r.startPageOfEntry != oldPage) {
      lastLine = addLine(0, 0, style().real("page-width"), 0,
                         QPen(QBrush(style().color("toc-line-color")),
                              style().real("toc-line-width")));
      headers << new SearchResItem(book->toc()->entry(r.startPageOfEntry),
                                   this);
      lastLine->setParentItem(headers.last());
      oldPage = r.startPageOfEntry;
      //connect(i, SIGNAL(vboxChanged()), SLOT(itemChanged()));
      connect(headers.last(), SIGNAL(clicked(int)),
              SIGNAL(pageNumberClicked(int)));
      if (y > y0 && y + headers.last()->childrenBoundingRect().height() > y1) {
        y = y0;
        sheet += 1;
      }
      headers.last()->setPos(0, y);
      sheetnos << sheet;
      y += headers.last()->childrenBoundingRect().height();
    }

    headers.last()->addResult(r, headers.last());
    y = headers.last()->pos().y()
      + headers.last()->childrenBoundingRect().height();
    lastLine->setPos(0, headers.last()->childrenBoundingRect().height());
    if (headers.last()->pos().y() > y0 && y > y1) { // move to next sheet
      headers.last()->setPos(0, y0);
      sheetnos.last() += 1;
      sheet += 1;
      y = y0 + headers.last()->childrenBoundingRect().height();
    }
  }
  nSheets = sheet+1;

  gotoSheet(0);
  
}

QString SearchResultScene::title() const { 
  return ttl;
}

void SearchResultScene::gotoSheet(int sheet) {
  for (int n=0; n<headers.size(); n++) 
    if (sheetnos[n] == sheet)
      headers[n]->show();
    else
      headers[n]->hide();
  BaseScene::gotoSheet(sheet);
}

QString SearchResultScene::pgNoToString(int n) const {
  return Roman(n).lc();
}
  
void SearchResultScene::makeContdItems() {
  // Simply don't make them??
  BaseScene::makeContdItems();
  contdItem->setPos(4, style().real("margin-top"));
}
