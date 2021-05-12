
// Data/LateNoteData.cpp - This file is part of NotedELN

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

// LateNoteData.C

#include "LateNoteData.h"
#include <QDebug>
#include "ResManager.h"

static Data::Creator<LateNoteData> c("latenote");

LateNoteData::LateNoteData(Data *parent): GfxNoteData(0) {
  setType("latenote");
  if (parent)
    parent->addChild(this, InternalMod);
  nb = 0;
}

LateNoteData::~LateNoteData() {
  Data *p = parent();
  if (p)
    p->takeChild(this, NonPropMod);
}

void LateNoteData::markModified(ModType mt) {
  if (mt==UserVisibleMod)
    mt = NonPropMod;
  GfxNoteData::markModified(mt);
}

void LateNoteData::setBook(Notebook *b) {
  nb = b;
}

Notebook *LateNoteData::book() const {
  return nb ? nb : GfxNoteData::book();
}

ResManager *LateNoteData::resManager() const {
  ResManager *r0 = GfxNoteData::resManager();
  if (r0)
    return r0;
  return firstChild<ResManager>();
}
