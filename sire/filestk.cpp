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

   File  : filestk.cpp
   Class : TfileStk
   Desc  : A stack of open files. The files are represented internals as
           TfileDat objects.

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "filedat.h"
#include "filestk.h"


/************* FUNCTIONS ****************************************************/





int TfileStk::get()
{
   int ch = EOF;
   
   if (!DungetStack.empty())
   {
      ch = DungetStack.pop();
   }
   
   while (!empty() && (EOF == ch))
   {
      TfileDat *fp = (TfileDat*)peek();
      
      if (NULL != fp) ch = fp->get();
      
      if (EOF == ch)
         pop();
   }

   return(ch);
}


char const *TfileStk::getLine(char *buff, int len)
{
   char const *ch = NULL;
   
   if (!DungetStack.empty())
   {
      ch = buff;
      
      while (!DungetStack.empty() && ('\n' != (*(buff++) = DungetStack.pop())))
         len--;
      
      *buff = '\0';
      
      if (('\n' != *(buff -1)) && !empty())
      {
         TfileDat   *fp = (TfileDat*)peek();
   
         fp->getLine(buff, len);
      }
   }
   
   while (!empty() && (NULL == ch))
   {
      TfileDat   *fp = (TfileDat*)peek();
   
      if (NULL != fp) ch = fp->getLine(buff, len);
      
      if (NULL == ch)
         pop();
   }

   return(ch);
}


char const *TfileStk::posn()
{
   if (!empty())
   {
      TfileDat *fp = (TfileDat*)peek();
      if (NULL != fp)
         return(fp->posn());
   }
         
   return("[0:0] <>");
}



TfilePos &TfileStk::tell()
{
   static TfilePos pos;
   
   if (!empty())
   {
      TfileDat *fp = (TfileDat*)peek();
      if (NULL != fp)
         return(fp->tell());
   }
         
   return(pos);
}



int TfileStk::seek(TfilePos const &pos)
{
   if (!empty())
   {
      TfileDat *fp = (TfileDat*)peek();
      if (NULL != fp)
         return(fp->seek(pos));
   }
         
   return(1);
}


void TfileStk::push(FILE *fp)       {TadrStack::push(new TfileDat(fp));}
void TfileStk::push(text  ip)       {TadrStack::push(new TfileDat(ip));}
void TfileStk::push(char const *nm) {TadrStack::push(new TfileDat(nm));}
void TfileStk::push(Tpipe      *p)  {TadrStack::push(new TfileDat(p));}


void TfileStk::pop()
{
   if (!empty())
   {
      TfileDat *fp = (TfileDat*)TadrStack::pop();
      if (NULL != fp) delete fp;
   }
}


void TfileStk::flush()
{
   while (!empty())  {pop();}
}


void TfileStk::putBack(char const *text)
{
   if (NULL != text)
      for (int pos = strlen(text) -1; (0 <= pos); pos--) 
         DungetStack.push(text[pos]);
}


void TfileStk::putBack(char ch)
{
   DungetStack.push(ch);
}



/************* END OF FILE **************************************************/
