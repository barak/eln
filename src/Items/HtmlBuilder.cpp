// Items/HtmlBuilder.cpp - This file is part of NotedELN

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

// HtmlBuilder.cpp

#include "HtmlBuilder.h"
#include "TextData.h"
#include "MarkupEdges.h"

HtmlBuilder::HtmlBuilder(TextData const *src, int start, int end) {
  text = src->text();
  if (end<0)
    end = text.size();
  text = text.mid(start, end-start);
  
  MarkupEdges edges(src->markups());
  MarkupStyles style;
  foreach (int k, edges.keys()) 
    if (k<start)
      style = edges[k];
    else
      break;

  if (!edges.contains(start)) {
    nowedges << start;
    nowstyles << style;
  }

  foreach (int k, edges.keys()) {
    if (k>=start && k<end) {
      style = edges[k];
      nowedges << k;
      nowstyles << style;
    } else if (k>=end) {
      break;
    }
  }

  nowedges << end;

  QList<MarkupData::Style> allStyles;
  allStyles << MarkupData::Italic;
  allStyles << MarkupData::Bold;
  allStyles << MarkupData::Superscript;
  allStyles << MarkupData::Subscript;

  style = MarkupStyles();
  for (int n=0; n<nowedges.size()-1; n++) {
    QString bit = text.mid(nowedges[n]-start, nowedges[n+1]-nowedges[n]);
    MarkupStyles newst = nowstyles[n];
    // peel off stack anything that closes here
    while (!stack.isEmpty() && !newst.contains(stack.last())) {
      html += closingTag(stack.last());
      style.remove(stack.takeLast());
    }
    // anything that closes out of order needs to be peeled off too
    foreach (MarkupData::Style s, allStyles) {
      if (style.contains(s) && !newst.contains(s)) {
	while (true) {
	  html += closingTag(stack.last());
	  style.remove(stack.last());
	  if (stack.takeLast()==s)
	    break;
	}
      }
    }
    // add new stuff
    foreach (MarkupData::Style s, allStyles) {
      if (newst.contains(s) && !style.contains(s)) {
	html += openingTag(s);
	style.add(s);
	stack << s;
      }
    }
    html += bit;
  }

  // peel off rest
  while (!stack.isEmpty())
    html += closingTag(stack.takeLast());
}

QString HtmlBuilder::openingTag(MarkupData::Style s) {
  QString tag = tagName(s);
  if (tag.isEmpty())
    return "";
  else
    return "<" + tag + ">";
}

QString HtmlBuilder::closingTag(MarkupData::Style s) {
  QString tag = tagName(s);
  if (tag.isEmpty())
    return "";
  else
    return "</" + tag + ">";
}

QString HtmlBuilder::tagName(MarkupData::Style s) {
  switch (s) {
  case MarkupData::Italic:
    return "i";
  case MarkupData::Bold:
    return "b";
  case MarkupData::Superscript:
    return "sup";
  case MarkupData::Subscript:
    return "sub";
  default:
    return "";
  }
}
