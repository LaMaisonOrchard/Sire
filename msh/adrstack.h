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

   File  : adrstack.h
   Class : TadrStack
   Desc  : A general stack. This stack hold the addresses of data items and
           assumes nothing about the data or the pointers.

*****************************************************************************/


#ifndef ADRSTACK
#define ADRSTACK


#include "adrbuf.h"



class TadrStack
{
public:

   TadrStack ();
   ~TadrStack ();
   
   void  push(void *ch);
   void *pop();
   void *peek();
   void *peek(int);
   
   int  empty();
   int  depth();
   void flush();
   
   operator voidPtr * ();
   
private:
   
   TadrBuf       Dstack;
   unsigned long Dtop;
};

inline   TadrStack::TadrStack () : Dtop(0) {}
   
inline   void  TadrStack::push(void *ch) {Dstack[Dtop++] = ch;}
   
inline   int  TadrStack::empty() {return(0 == Dtop);}
inline   int  TadrStack::depth() {return(Dtop);}
inline   void TadrStack::flush() {Dtop = 0;}
   
inline   TadrStack::operator voidPtr * () {Dstack[Dtop] = NULL; return((void* const*)Dstack);}



#endif
