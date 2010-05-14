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
   Tjlex
   
DESC
   This is the lexical analyser for jake.
   
FILE
   jlex.h

*****************************************************************************/


#ifndef JLEX
#define JLEX


#include <ctype.h>
#include <string.h>
#include "lex.h"


static char const SEP_TEXT        = '"';
static char const SEP_CALL        = '`';
static char const SEP_WHICH_OPEN  = '<';
static char const SEP_WHICH_CLOSE = '>';
static char const SEP_O_BRK       = '{';
static char const SEP_C_BRK       = '}';
static char const SEP_O_SUB       = '[';
static char const SEP_C_SUB       = ']';
static char const SEP_S_VAR       = '$';
static char const SEP_ESC         = '\\';

static char *const SEP_S_VAR_STR  = "$";

int isIdent(int ch);
int isSymb (int ch);


class Tjlex : public Tlex
{
public:

   Tjlex (FILE *fp = stdin);
   Tjlex (text  ip);
   Tjlex (char const *nm);

   virtual Ttoken *getToken    ();
   
protected:

private:

   int  readComment(Ttoken &token);
   int  readIdent  (Ttoken &token);
   int  readBlock  (Ttoken &token);
   int  readSubComm(Ttoken &token);
   void readProc   ();
   int  readText   (Ttoken &token);
   int  readSymb   (Ttoken &token);
   int  readCall   (Ttoken &token);
   int  readWhich  (Ttoken &token);
   int  readVar    ();

   static char Dbuff[1024];
};

inline   Tjlex::Tjlex (FILE *fp) : Tlex(fp) {}
inline   Tjlex::Tjlex (text  ip) : Tlex(ip) {}
inline   Tjlex::Tjlex (char const *nm)   : Tlex(nm) {}



#endif
