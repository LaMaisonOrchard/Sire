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

   File  : tokenstk.h
   Class : TtokenStk
   Desc  : A stack of tokens. This is build apon the more general TadrStack.

*****************************************************************************/


#ifndef TOKENSTK
#define TOKENSTK

#include <stdio.h>

#include "adrstack.h"
#include "token.h"



class TtokenStk : public TadrStack
{
private:

public:

   TtokenStk (Ttoken *fp);
   TtokenStk ();
   ~TtokenStk();
   
   void    push(Ttoken *fp);
   Ttoken *pop ();
   void    flush();
};

inline   TtokenStk::TtokenStk (Ttoken *fp) {TadrStack::push(fp);}
inline   TtokenStk::TtokenStk ()           {}
inline   TtokenStk::~TtokenStk()           {}

inline   void TtokenStk::push(Ttoken *fp) {TadrStack::push(fp);}



#endif
