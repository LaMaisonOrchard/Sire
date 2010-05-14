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


*****************************************************************************

   File  : target.h
   Class : Ttarget
   Desc  : A description

*****************************************************************************/


#ifndef TARGET
#define TARGET


#include "str.h"
#include "list.h"

class Trule;

class Ttarget
{
public:

   Ttarget (char const *target, Trule *rule);
   virtual ~Ttarget();
   
   char const *target();
   Trule      *rule();
   
   int         date      (char const *name);
   int         buildable (char const *name);
   virtual int match     (char const *name);
   
   static int      find(char const *name, int start);
   
   static Ttarget *get (int idx);
   static int      len ();
   
protected:

private:

   Tstr   Dtarget;
   Trule *Drule;
   
   static Tlist Dlist;
};
   
inline   char const *Ttarget::target() {return((char const*)Dtarget);}
inline   Trule      *Ttarget::rule()   {return(Drule);}
   
inline   Ttarget *Ttarget::get (int idx)  {return((Ttarget*)Dlist[idx]);}
inline   int      Ttarget::len ()         {return(Dlist.len());}



#endif
