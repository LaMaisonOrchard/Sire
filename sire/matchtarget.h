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

   File  : matchtarget.h
   Class : TmatchTarget
   Desc  : A description

*****************************************************************************/


#ifndef MATCHTARGET
#define MATCHTARGET


#include "target.h"


class TmatchTarget : public Ttarget
{
public:

   TmatchTarget (char const *target, Trule *rule);
   virtual ~TmatchTarget();
   
   virtual int match(char const *name);
   
   static int  find (char const *name, int start);
   
   static Ttarget *get (int idx);
   static int      len ();
   
protected:

private:
   
   static Tlist Dlist;
};
   
inline Ttarget *TmatchTarget::get (int idx)  {return((Ttarget*)Dlist[idx]);}
inline int      TmatchTarget::len ()         {return(Dlist.len());}



#endif
