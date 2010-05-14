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

   File  : targetstmplst.h
   Class : TtargetStmpLst
   Desc  : A description

*****************************************************************************/


#ifndef TARGETSTMPLST
#define TARGETSTMPLST


#include "list.h"
#include "targetstamp.h"



class TtargetStmpLst : public Tlist
{
public:

   TtargetStmpLst () : Tlist() {}
   ~TtargetStmpLst() {}

   void          add(TtargetStamp *data)     {Tlist::add(data);}
   TtargetStamp *remove(int idx)       {return((TtargetStamp*)Tlist::remove(idx));}
   TtargetStamp *operator [] (int idx) {return((TtargetStamp*)Tlist::operator [] (idx));}
   TtargetStamp *find(char const *data);
   
protected:

private:

};



#endif
