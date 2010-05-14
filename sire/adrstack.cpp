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

   File  : adrstack.cpp
   Class : TadrStack
   Desc  : A general stack. This stack hold the addresses of data items and
           assumes nothing about the data or the pointers.

*****************************************************************************/




/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "adrstack.h"


/************* FUNCTIONS ****************************************************/



TadrStack::~TadrStack()
{
}


void *TadrStack::pop()
{
   if (!empty())
      return(Dstack[--Dtop]);
   else
      return(NULL);
}

void *TadrStack::peek()
{
   if (!empty())
      return(Dstack[Dtop-1]);
   else
      return(NULL);
}

void *TadrStack::peek(int idx)
{
   if (!empty())
      return(Dstack[(unsigned long)idx]);
   else
      return(NULL);
}



/************* END OF FILE **************************************************/
