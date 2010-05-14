/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999  David W Orchard (davido@errol.org.uk)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/


#include <stdlib.h>

#include "str.h"

static char emptyStr[1] = "";



void Tstr::stringFree(char *s)
{
   if ((NULL != s) && (emptyStr != s)) delete s;
}



char *Tstr::stringAlloc(char const *s)
{
   char *t;

   if ((NULL == s) || ('\0' == *s))
   {
      t = emptyStr;
   }
   else
   {
      t = new char [strlen(s) +1];
      strcpy(t,s);
   }

   return(t);
}


Tstr &Tstr::operator +=  (char const *str)
{
    if (NULL == Dtext)
    {
        Dtext = stringAlloc(str);
    }
    else if (NULL == str)
    {
        // Nothing to do
    }
    else
    {
        char *old = Dtext;
        Dtext  = new char[strlen(old) + strlen(str) +1];
        strcpy(Dtext, old);
        strcat(Dtext, str);
        delete old;
    }

    return (*this);
}



char const *strText(char const *str)
{
   if (NULL == str)
      return("");
   else
      return(str);
}


int strEqual(char const *s1, char const *s2)
{
   return (!strcmp(strText(s1),strText(s2)));
}


int strNEqual(char const *s1, char const *s2)
{
   return (strcmp(strText(s1),strText(s2)));
}


int strCmp(char const *s1, char const *s2)
{
   return (strcmp(strText(s1),strText(s2)));
}


