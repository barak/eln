// Book/Style.H - This file is part of NotedELN

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

// Style.H

#ifndef STYLE_H

#define STYLE_H

#include <QVariant>
#include <QFont>

class Style {
public:
  Style(QString fn);
  QVariant operator[](QString) const;
  double real(QString) const;
  double real(QString, double dflt) const;
  QString string(QString) const;
  QString string(QString, QString dflt) const;
  bool flag(QString) const;
  int integer(QString) const;
  QColor color(QString) const;
  QColor alphaColor(QString) const;
  QFont font(QString) const;
  double lineSpacing(QString font, QString scale) const;
  double lineSpacing(QString font, double scale) const;
  QVariantMap const &options() const;
  bool contains(QString) const;
  static Style const &defaultStyle();
private:
  Style();
  QVariantMap options_;
};

#endif
