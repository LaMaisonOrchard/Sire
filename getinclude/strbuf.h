/****************************************************************************

    The Sire include file extarction utility 'getinclude'.
    Copyright (C) 1999, 2002, 2004  David W Orchard (davido@errol.org.uk)

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

#ifndef STRBUF
#define STRBUF


#include <stdlib.h>



class TstrBuf
{
public:

   TstrBuf ();
   TstrBuf (TstrBuf const &buff);
   TstrBuf (char const *str);
   ~TstrBuf();

   char    &operator [] (unsigned long idx);
   TstrBuf &operator =  (char const *str);
   TstrBuf &operator =  (TstrBuf const &buf);
   long     len() {return(Dlen);}
   
   TstrBuf &cat(char const *text, int start = 0);
   
   operator char const * ();
   
private:

   char          *Dbuff;  // Never == NULL
   unsigned long  Dlen;
};



inline   TstrBuf::TstrBuf () : Dbuff(new char[10]), Dlen(10) {}
inline   TstrBuf::operator char const * () {return(Dbuff);}



#endif
