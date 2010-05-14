/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2000, 2001  David W Orchard (davido@errol.org.uk)

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

   File  : filedat.cpp
   Class : TfileDat
   Desc  : A description
   
AMENDMENTS

EDIT 001 27-08-02 davido@errol.org.uk
Add support for preprocessing.

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "filedat.h"



static int c1    = 0;  // First  charactor in the file
static int c2    = 0;  // Second charactor in the file

int TfileDat::PreProcess = 0;


/************* FUNCTIONS ****************************************************/



TfileDat::TfileDat (FILE *fp) : Dfile(fp), Dinput(""), Dpipe(NULL), Didx(-1), Dname(""), Dline(0), Dcolm(0)
{
   if (NULL != Dfile)
   {
      c1 = getc(Dfile);
      c2 = getc(Dfile);
   
      if (('#' == c1) && ('!' == c2))
      {
         while ((EOF != c1) && ('\n' != c1))
         {
            c1 = getc(Dfile);
         }
         c1 = c2 = 0;
      }
   }
}



TfileDat::TfileDat (Tpipe *p) : Dfile(p->file()), Dinput(""), Dpipe(p), Didx(-1), Dname(""), Dline(0), Dcolm(0)
{
   if (NULL != Dfile)
   {
      c1 = getc(Dfile);
      c2 = getc(Dfile);
   
      if (('#' == c1) && ('!' == c2))
      {
         while ((EOF != c1) && ('\n' != c1))
         {
            c1 = getc(Dfile);
         }
         c1 = c2 = 0;
      }
   }
}


TfileDat::TfileDat (text input) : Dfile(NULL), Dinput(input), Dpipe(NULL), Didx(0), Dname(""), Dline(0), Dcolm(0)
{
   if (NULL != Dfile)
   {
      c1 = Dinput[Didx++];
      c2 = Dinput[Didx++];
   
      if (('#' == c1) && ('!' == c2))
      {
         while (('\0' != c1) && ('\n' != c1))
         {
            c1 = Dinput[Didx++];
         }
         
         if ('\0' == c1) Didx--;
      }
      else
      {
         Didx = 0;
      }
      c1 = c2 = 0;
   }
}



TfileDat::TfileDat (char const *nm) : Dfile(NULL), Dinput(""), Dpipe(NULL), Didx(-1), Dname(nm), Dline(0), Dcolm(0)
{
   if (NULL != nm)
   {
      char *p;
      char buff[1024];
      
      if ((NULL != (p = strrchr(nm, '.'))) && !strcmp(p, ".Z"))
      {
         // Compressed file
         
         sprintf (buff, "zcat %s", nm);
         if (PreProcess) strcat(buff, " | " PREPROCESSOR);   // 001
         Dpipe = new Tpipe(buff);
         Dfile = Dpipe->file();
      }
      else if ((NULL != (p = strrchr(nm, '.'))) && !strcmp(p, ".gz"))
      {
         // Compressed file
         
         sprintf (buff, "gzip -dc %s", nm);
         if (PreProcess) strcat(buff, " | " PREPROCESSOR);   // 001
         Dpipe = new Tpipe(buff);
         Dfile = Dpipe->file();
      }
      else if ((NULL != (p = strrchr(nm, '.'))) && !strcmp(p, ".gzip"))
      {
         // Compressed file
         
         sprintf (buff, "gzip -dc %s", nm);
         if (PreProcess) strcat(buff, " | " PREPROCESSOR);   // 001
         Dpipe = new Tpipe(buff);
         Dfile = Dpipe->file();
      }
      else if (PreProcess)   // 001
      {
         sprintf (buff, PREPROCESSOR " %s", nm);
         Dpipe = new Tpipe(buff);
         Dfile = Dpipe->file();
      }
      else
      {
         // Regular file
         Dfile = fopen(nm, "r");
      }
      
      if ((NULL == Dfile) && (NULL == Dpipe))
      {
         printf("Failed to open file <%s>\n", nm);
      }
   }
   else
   {
      nm = "";
   }
   
   if (NULL != Dfile)
   {
      c1 = getc(Dfile);
      c2 = getc(Dfile);
   
      if (('#' == c1) && ('!' == c2))
      {
         while ((EOF != c1) && ('\n' != c1))
         {
            c1 = getc(Dfile);
         }
         c1 = c2 = 0;
      }
   }
}


TfileDat::~TfileDat()
{
   if (NULL != Dpipe)
   {
      delete Dpipe;
   }
   else if (NULL != Dfile)
   {
      fclose(Dfile);
   }
}


int TfileDat::get()
{
   register int ch;
   
   if (0 != c1) {ch = c1; c1 = 0 ; return (ch);}
   if (0 != c2) {ch = c2; c2 = 0 ; return (ch);}
   
   if (NULL != Dfile)
   {
      ch = getc(Dfile);
      
      if ('\n' == ch)
      {
         Dline++;
         Dcolm = 0;
      }
      else if (EOF == ch)
      {
         if (NULL != Dpipe)
         {
            delete Dpipe;
            Dpipe = NULL;
         }
         else if (NULL != Dfile)
         {
            fclose(Dfile);
         }
         Dfile = NULL;
      }
      else
      {
         Dcolm++;
      }
      return(ch);
   }
   else if (0 <= Didx)
   {
      ch = Dinput[Didx++];
      
      if ('\n' == ch)
      {
         Dline++;
         Dcolm = 0;
      }
      else if ('\0' == ch)
      {
         Didx = -1;
         ch   = EOF;
      }
      else
      {
         Dcolm++;
      }
      return(ch);
   }
   else
   {
      return(EOF);
   }
}


char const *TfileDat::getLine(char *buff, int len)
{
   char *localBuff = buff;
   char  ch        = get();
   
   if (EOF != ch)
   {
      while ((EOF != ch) && ('\n' != ch))
      {
         *(localBuff++) = ch;
         ch = get();
      }
      
      if (EOF != ch) *(localBuff++) = ch;
      
      *(localBuff++) = '\0';
      
      return(buff);
   }
   else
   {
      return(NULL);
   }
}


char const *TfileDat::posn()
{
   static char buff[1024];
   
   sprintf(buff, "[%ld:%ld] <%s>", Dline, Dcolm, (char const*)Dname);
   return(buff);
}


TfilePos &TfileDat::tell()
{
   static TfilePos pos;
   
   pos.Dobject = this;
   
   if (NULL != Dfile)
      pos.Dposn = ftell(Dfile);
   else if (0 <= Didx)
      pos.Dposn = Didx;
   else
      pos.Dposn = 0;
      
   return(pos);
}


int TfileDat::seek(TfilePos const &pos)
{
   if (this != pos.Dobject)
      return(1);
   else if (NULL != Dfile)
      fseek(Dfile,pos.Dposn, 0);
   else if (0 <= Didx)
      Didx = pos.Dposn;
   else
      return(1);
      
   return(0);
}



/************* END OF FILE **************************************************/
