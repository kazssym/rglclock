/* rglclock - Rotating GL Clock.
   Copyright (C) 1998, 1999 Hypercore Software Design, Ltd.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.

   As a special exception, you may copy and distribute this program
   linked with an OpenGL library in executable form without the source
   code for the library, only if such distribution is not prohibited
   by the library's license.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#undef const

#include "profile.h"

#ifdef HAVE_GNOME_XML_PARSER_H
# include <cstdio>
# include <gnome-xml/parser.h>
#endif

using namespace std;

void
profile::save(glclock *clock)
{
#ifdef HAVE_GNOME_XML_PARSER_H
  xmlDocPtr doc = xmlNewDoc("1.0");

  try
    {
      xmlNodePtr root = xmlNewDocNode(doc, NULL, PACKAGE, NULL);
      xmlNodePtr misc = xmlNewChild(root, NULL, "misc", NULL);
      char v[10];
      sprintf(v, "%d", clock->update_rate());
      xmlNewProp(misc, "update", v);

      xmlSaveFile(file_name.c_str(), doc);
    }
  catch (...)
    {
      xmlFreeDoc(doc);
      throw;
    }

  xmlFreeDoc(doc);
#endif /* HAVE_GNOME_XML_PARSER_H */
}

void
profile::restore(glclock *clock)
{
#ifdef HAVE_GNOME_XML_PARSER_H
  xmlDocPtr doc = xmlParseFile(file_name.c_str());
  if (doc == NULL)
    return;

  try
    {
      xmlNodePtr n = doc->root->childs;
      while (n != NULL && strcmp(n->name, "misc") != 0)
	n = n->next;

      if (n != NULL)
	{
	  xmlAttrPtr a = n->properties;
	  while (a != NULL && strcmp(a->name, "update") != 0)
	    ++a;

	  if (a != NULL)
	    {
	      int rate = atoi(reinterpret_cast<char *>(a->val->content));
	      if (rate > 0)
		clock->set_update_rate(rate);
	    }
	}
    }
  catch (...)
    {
      xmlFreeDoc(doc);
      throw;
    }

  xmlFreeDoc(doc);
#endif /* HAVE_GNOME_XML_PARSER_H */
}

profile::~profile()
{
}

profile::profile(const char *name)
  : file_name(name)
{
}

