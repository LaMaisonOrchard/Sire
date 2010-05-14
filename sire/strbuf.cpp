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

   File  : strbuff.cpp
   Class : TstrBuf
   Desc  : A description
   
   NOTE  : Dbuff is guaranteed never to be NULL

EDIT 001 18-Apr-96 davido
Modify the default opperation for 'cat'.

*****************************************************************************/




/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strbuf.h"


/************* FUNCTIONS ****************************************************/



TstrBuf::TstrBuf(TstrBuf const &buff) : Dbuff(NULL), Dlen(0)
{
   Dlen  = buff.Dlen;
   Dbuff = new char [Dlen];
   for (unsigned long i = 0; (i < Dlen); i++) Dbuff[i] = buff.Dbuff[i];
}


TstrBuf::TstrBuf(char const *str) : Dbuff(NULL), Dlen(0)
{
   if (NULL != str)
      Dlen  = strlen(str) +1;
   else
      Dlen  = 10;
      
   Dbuff = new char [Dlen];
   
   if (NULL != str)
      strcpy(Dbuff,str);
}


TstrBuf::~TstrBuf() {delete Dbuff;}

char &TstrBuf::operator [] (unsigned long idx)
{
   if (idx >= Dlen)
   {
      char *tmp = new char [2*idx +20];
      for (unsigned long i = 0; (i < Dlen); i++) tmp[i] = Dbuff[i];
      delete Dbuff;
      Dbuff = tmp;
      Dlen  = 2*idx +20;
   }
   return(Dbuff[idx]);
}

TstrBuf &TstrBuf::operator = (char const *str)
{
   if (NULL == str)
   {
      operator [](0) = '\0';
   }
   else if (Dbuff != str)
   {
      // Ensure enophe space
      operator []((unsigned long)strlen(str) +1) = '\0';
      
      // Copy the text
      strcpy(Dbuff, str);
   }
   
   return(*this);
}

TstrBuf &TstrBuf::operator = (TstrBuf const &buf)
{
   if (Dlen < buf.Dlen)
   {
      delete Dbuff;
      Dbuff = new char [buf.Dlen];
      Dlen  = buf.Dlen;
   }
   
   for (unsigned long i = 0; (i < buf.Dlen); i++) Dbuff[i] = buf.Dbuff[i];
   
   return(*this);
}

TstrBuf &TstrBuf::cat(char const *str, int start)
{
   if (NULL != str)
   {
      if (0 > start) start = strlen(Dbuff); // 001
      if ((Dbuff + start) != str)
      {
         // Ensure enophe space
         operator []((unsigned long )(start + strlen(str) +1)) = '\0';
      
         // Copy the text
         strcpy(Dbuff + start, str);
      }
   }
   
   return(*this);
}



/************* END OF FILE **************************************************/
