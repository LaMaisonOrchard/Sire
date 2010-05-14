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

   File  : deplist.h
   Class : TdepList
   Desc  : A description

*****************************************************************************/


#ifndef DEPLIST
#define DEPLIST


#include "list.h"


class Tdep;

class TdepList : public Tlist
{
public:

   TdepList () : Tlist() {}
   ~TdepList();

   void  add(Tdep *data)       {Tlist::add(data);}
   Tdep *remove(int idx)       {return((Tdep*)Tlist::remove(idx));}
   Tdep *operator [] (int idx) {return((Tdep*)Tlist::operator [] (idx));}
   
protected:

private:

};



#endif
