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

   File  : adrbuff.cpp
   Class : TadrBuf
   Desc  : A description

*****************************************************************************/




/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "adrbuf.h"

/************* FUNCTIONS ****************************************************/



TadrBuf::TadrBuf(TadrBuf const &buff) : Dbuff(NULL), Dlen(0)
{
   if (NULL != buff.Dbuff)
   {
      Dlen = buff.Dlen;
      Dbuff = new void* [Dlen];
      for (unsigned long i = 0; (i < Dlen); i++) Dbuff[i] = buff.Dbuff[i];
   }
}


TadrBuf::~TadrBuf() {if (NULL != Dbuff) delete Dbuff;}

void* &TadrBuf::operator [] (unsigned long idx)
{
   if ((NULL == Dbuff)||(idx >= Dlen))
   {
      void **tmp = new void*[2*idx +20];
      for (unsigned long i = 0; (i < Dlen); i++) tmp[i] = Dbuff[i];
      delete Dbuff;
      Dbuff = tmp;
      Dlen  = 2*idx +20;
   }
   return(Dbuff[idx]);
}





/************* END OF FILE **************************************************/
