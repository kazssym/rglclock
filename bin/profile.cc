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

#include <iterator>
#ifdef HAVE_GNOME_XML_PARSER_H
# include <cstdio>
# include <gnome-xml/parser.h>
#endif

using namespace std;

namespace
{
#ifdef HAVE_GNOME_XML_PARSER_H
  class xmlNode_iterator
    : public bidirectional_iterator<xmlNode, ptrdiff_t>
  {
    friend bool operator==(const xmlNode_iterator &a,
			   const xmlNode_iterator &b)
      {return a.ptr == b.ptr;}
  private:
    xmlNode *ptr;
  public:
    xmlNode_iterator()
      : ptr(NULL) {}
    xmlNode_iterator(xmlNode *p)
      : ptr(p) {}
  public:
    xmlNode &operator*() const
      {return *ptr;}
    xmlNode *operator->() const
      {return ptr;}
    xmlNode_iterator &operator++()
      {ptr = ptr->next; return *this;}
    xmlNode_iterator operator++(int)
      {xmlNode_iterator tmp = *this; ++*this; return tmp;}
    xmlNode_iterator &operator--()
      {ptr = ptr->prev; return *this;}
    xmlNode_iterator operator--(int)
      {xmlNode_iterator tmp = *this; --*this; return tmp;}
  };
#endif /* HAVE_GNOME_XML_PARSER_H */
} // (unnamed namespace)

void
profile::options_changed(glclock *clock)
{
  changed = true;
}

void
profile::save(glclock *clock)
{
  if (changed)
    {
      changed = false;

#ifdef HAVE_GNOME_XML_PARSER_H
      xmlDocPtr doc = xmlParseFile(file_name.c_str());
      if (doc == NULL)
	doc = xmlNewDoc(reinterpret_cast<CHAR *>("1.0"));

      try
	{
	  if (doc->root == NULL)
	    {
	      doc->root = xmlNewDocNode(doc, NULL,
					reinterpret_cast<CHAR *>(PACKAGE),
					NULL);
	      xmlNewChild(doc->root, NULL,
			  reinterpret_cast<CHAR *>("misc"), NULL);
	    }

	  for (xmlNode_iterator i = doc->root->childs;
	       i != xmlNode_iterator();
	       ++i)
	    {
	      if (xmlStrcmp(i->name, reinterpret_cast<CHAR *>("misc")) == 0)
		{
		  char v[10];
		  sprintf(v, "%d", clock->update_rate());
		  xmlSetProp(&*i, reinterpret_cast<CHAR *>("update"),
			     reinterpret_cast<CHAR *>(v));
		}
	    }

	  string tmp(file_name);
	  tmp.append(".tmp");
	  xmlSaveFile(tmp.c_str(), doc);
	  rename(tmp.c_str(), file_name.c_str());
	}
      catch (...)
	{
	  xmlFreeDoc(doc);
	  throw;
	}

      xmlFreeDoc(doc);
#endif /* HAVE_GNOME_XML_PARSER_H */
    }
}

void
profile::restore(glclock *clock)
{
  changed = false;

#ifdef HAVE_GNOME_XML_PARSER_H
  xmlDocPtr doc = xmlParseFile(file_name.c_str());
  if (doc == NULL)
    return;

  try
    {
      for (xmlNode_iterator i = doc->root->childs;
	   i != xmlNode_iterator();
	   ++i)
	{
	  if (xmlStrcmp(i->name, reinterpret_cast<CHAR *>("misc")) == 0)
	    {
	      const CHAR *value
		= xmlGetProp(&*i, reinterpret_cast<CHAR *>("update"));
	      if (value != NULL && value[0] != '\0')
		{
		  int rate = atoi(reinterpret_cast<const char *>(value));
		  clock->set_update_rate(rate);
		}
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

void
profile::open(const char *name)
{
  file_name = name;
}

profile::~profile()
{
}

profile::profile()
  : changed(false)
{
}

