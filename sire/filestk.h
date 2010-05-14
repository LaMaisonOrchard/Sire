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

   File  : filestk.h
   Class : TfileStk
   Desc  : A stack of open files. The files are represented internals as
           TfileDat objects.

*****************************************************************************/


#ifndef FILESTK
#define FILESTK

#include <stdio.h>

#include "adrstack.h"
#include "charstack.h"
#include "filedat.h"



class TfileStk : public TadrStack
{
public:

   TfileStk (FILE       *fp = stdin);
   TfileStk (text        ip);
   TfileStk (char const *nm);
   TfileStk (Tpipe      *pipe);
   virtual ~TfileStk();
   
   int         get();
   char const *getLine(char *buff, int len);
   char const *posn();
   
   void push(FILE *fp);
   void push(text  ip);
   void push(char const *nm);
   void push(Tpipe      *pipe);
   void pop();
   void flush();
   
   void putBack(char const *text);
   void putBack(char ch);
   
   TfilePos &tell();
   int       seek(TfilePos const &pos);
   
protected:

private:

   TcharStack DungetStack;

};

inline TfileStk::TfileStk (FILE *fp)       {push(fp);}
inline TfileStk::TfileStk (text  ip)       {push(ip);}
inline TfileStk::TfileStk (char const *nm) {push(nm);}
inline TfileStk::TfileStk (Tpipe      *p)  {push(p);}
inline TfileStk::~TfileStk ()              {}



#endif
