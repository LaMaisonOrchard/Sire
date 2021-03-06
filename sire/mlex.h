/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2002  David W Orchard (davido@errol.org.uk)

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

CLASS
   Tmlex
   
DESC
   This is the lexical analyser for jake.
   
FILE
   mlex.h

*****************************************************************************/


#ifndef MLEX
#define MLEX


#include <ctype.h>
#include <string.h>
#include "jlex.h"


class Tmlex : public Tlex
{
public:

   Tmlex (FILE *fp = stdin);
   Tmlex (text  ip);
   Tmlex (char const *nm);

   virtual Ttoken *getToken    ();

    static int isIdent(int ch);
    static int isSymb (int ch);
   
protected:

private:

   int  readComment(Ttoken &token);
   int  readIdent  (Ttoken &token);
   int  readText   (Ttoken &token);
   int  readCall   (Ttoken &token);
   int  readSymb   (Ttoken &token);
   int  readVar    ();


   static char Dbuff[1024];
};

inline   Tmlex::Tmlex (FILE *fp) : Tlex(fp) {}
inline   Tmlex::Tmlex (text  ip) : Tlex(ip) {}
inline   Tmlex::Tmlex (char const *nm)   : Tlex(nm) {}



#endif
