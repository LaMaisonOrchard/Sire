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

   File  : targetstamp.h
   Class : TtargetStamp
   Desc  : A description

*****************************************************************************/


#ifndef TARGETSTAMP
#define TARGETSTAMP


#include "str.h"



class TtargetStamp
{
public:

   TtargetStamp (char const *target, int stamp) : Dtarget(target), Dstamp(stamp) {}

   int stamp() {return(Dstamp);}
   
   int operator == (char const *target);
   int operator != (char const *target);
      
protected:

private:

   Tstr Dtarget;
   int  Dstamp;

};
   
inline   int TtargetStamp::operator == (char const *target) {return(Dtarget == target);}
inline   int TtargetStamp::operator != (char const *target) {return(Dtarget != target);}



#endif
