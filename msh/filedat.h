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

   File  : filedat.h
   Class : TfileDat
   Desc  : A description
   
AMENDMENTS

EDIT 001 27-08-02 davido@errol.org.uk
Add support for preprocessing.

*****************************************************************************/


#ifndef FILEDAT
#define FILEDAT


#include "str.h"
#include "pipe.h"
#include "filepos.h"


#define PREPROCESSOR "m4"   // 001

// This is a 'phantom' class to distinguish name text from input text.

class text
{
   private :
   
   char const *Ddata;
   
   public :
   
   text (text const &t) : Ddata(t.Ddata) {};
   text (char const *t) : Ddata(t)       {};
   operator char const *() const {return(Ddata);}
};







// Support file input for lexical analysis


class TfileDat
{
public:

   TfileDat (FILE  *fp);
   TfileDat (char   const *nm);
   TfileDat (text   input);
   TfileDat (Tpipe *pipe);
   ~TfileDat();
   
   int         get();
   char const *getLine(char *buff, int len);
   char const *posn();
   
   TfilePos   &tell();
   int         seek(TfilePos const &pos);
   
   static void preprocess(int a = 1);   // 001
   
protected:

private:

   FILE  *Dfile;
   Tstr   Dinput;
   Tpipe *Dpipe;
   long   Didx;
   Tstr   Dname;
   long   Dline;
   long   Dcolm;
   
   static int PreProcess;   // 001
};


   
inline void TfileDat::preprocess(int a) {PreProcess = a;}   // 001

#endif
