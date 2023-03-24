// ResourceMagic/AN_Biblio.cpp - This file is part of NotedELN

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

// AN_Biblio.C

#include "AN_Biblio.h"

#include "JSONFile.h"
#include "Style.h"

#include <QFileInfo>
#include <QDateTime>
#include <QVariantMap>
#include <QRegularExpression>
#include <QDir>
#include <QDebug>

QVariantMap const &AN_Biblio::biblio(Style const &st) {
  static QVariantMap empty;
  static QMap<QString, QVariantMap> bbls;
  static QMap<QString, QDateTime> lastloaded;
  QString k = st.string("bib-file", "");
  if (k.isEmpty())
    return empty;
  QDateTime lastmod = QFileInfo(k).lastModified();
  if (!bbls.contains(k) || lastmod>lastloaded[k]) {
    bbls[k] = JSONFile::load(k);
    lastloaded[k] = lastmod;
  }
  qDebug() << "Got biblio for" << k;
  return bbls[k];
}

AN_Biblio::AN_Biblio(QString tag, Style const &st) {
  if (tag.indexOf(QRegularExpression("[A-Z]"))==0)
    tag_ = tag.mid(1);
  else
    tag_ = tag;
  
  QVariantMap const &bbl(biblio(st));
  if (bbl.contains(tag_))
    ref_ = bbl[tag_].toString();
  
  if (st.contains("bib-dir")) {
    QDir dir(st.string("bib-dir"));
    if (dir.exists(tag + ".pdf"))
      url_ = QUrl::fromLocalFile(dir.absoluteFilePath(tag + ".pdf"));
    else if (tag_!=tag && dir.exists(tag_ + ".pdf"))
      url_ = QUrl::fromLocalFile(dir.absoluteFilePath(tag_ + ".pdf"));
  } // otherwise, url_ will be null

  qDebug() << "AN_Biblio" << tag << ref_ << url_;
}

bool AN_Biblio::ok() const {
  return !ref_.isEmpty();
}

QString AN_Biblio::ref() const {
  return ref_;
}

QUrl AN_Biblio::url() const {
  return url_;
}
