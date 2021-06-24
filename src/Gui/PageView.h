// App/PageView.H - This file is part of NotedELN

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

// PageView.H

#ifndef PAGEVIEW_H

#define PAGEVIEW_H

#include <QGraphicsView>
#include <QMap>
#include "CachedPointer.h"
#include <QDropEvent>
#include "Mode.h"

class PageView: public QGraphicsView {
  Q_OBJECT;
public:
  PageView(class SceneBank *bank, class PageEditor *parent);
  virtual ~PageView();
public:
  enum Section {
    Front,
    TOC,
    Entries,
  };
  class SavedState {
    friend class PageView;
  public:
    SavedState();
  private:
    Section section;
    int sheet;
    int entryStart;
    QString entryID;
    Mode::M mode;
    QString textItemID;
    int cursorPos;
  };
public:
  Section section() const;
  QString pageName() const;
  int pageNumber() const;
  SavedState saveState() const;
  void restoreState(SavedState const &);
public slots:
  void pageNumberClick(int, Qt::KeyboardModifiers);
  PageView *newView(); // opens new view on a named entry page
  void gotoEntryPage(QString pgno, QString path);
  void gotoEntryPage(QString pgno);
  void gotoEntryPage(int pgno, int dir=0);
  void gotoTOC(int pgno=-1);
  void goTOC(Qt::KeyboardModifiers=0);
  void gotoFront();
  void nextPage();
  void goRelative(int n, Qt::KeyboardModifiers=0);
  void previousPage();
  void lastPage(Qt::KeyboardModifiers m=0);
  void newPage(Qt::KeyboardModifiers m=0);
  void openFindDialog();
  void openGotoPageDialog();
  void htmlDialog();
  void drop(QDropEvent);
  void ensureSearchVisible(QString uuid, QString phrase);
  void openPrintDialog();
signals:
  void onEntryPage(int p0, int dp); // start page of entry, sheet no
  void onFrontMatter(int);
  void scaled(double);
public:
  class Mode *mode() const;
  class Notebook *notebook() const;
  void markCursor(QPointF, QFont, QColor);
protected:
  virtual void resizeEvent(QResizeEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  virtual void keyReleaseEvent(QKeyEvent *);
  virtual void mousePressEvent(QMouseEvent *);
  virtual void wheelEvent(QWheelEvent *);
  virtual void dragEnterEvent(QDragEnterEvent *);
  virtual void enterEvent(QEvent *);
  virtual void leaveEvent(QEvent *);
  virtual void inputMethodEvent(QInputMethodEvent *);
  virtual void focusInEvent(QFocusEvent *);
  virtual void focusOutEvent(QFocusEvent *);
  //  void dragMoveEvent(QDragMoveEvent *);
  virtual void drawBackground(QPainter *, QRectF const &);
  virtual void drawForeground(QPainter *, QRectF const &);
private:
  void leavePage();
  void createContinuationEntry();
  void focusEntry();
private slots:
  bool gotoSheet(int n);
  void handleSheetRequest(int n);
  void modeChange();
  void emptyEntryChange();
private:
  class SceneBank *bank; // we do not own!
  class Notebook *book; // we do not own!
  class Mode *mode_; // we own
  CachedPointer<class EntryScene> entryScene;
  int currentPage;
  int currentSheet;
  Section currentSection;
  class DeletedStack *deletedStack;
  int wheelDeltaAccum;
  int wheelDeltaStepSize;
  class SearchDialog *searchDialog;
  struct {
    QFont font;
    QColor color;
    QPointF pos; // null for invalid
  } cursorDrawer;
};

#endif
