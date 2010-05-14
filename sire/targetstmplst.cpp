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

   File  : targetstmplst.cpp
   Class : TtargetStmpLst
   Desc  : A description

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "targetstmplst.h"


/************* FUNCTIONS ****************************************************/


TtargetStamp *TtargetStmpLst::find(char const *data)
{
    int i;
    for (i = len() -1; (0 <= i); i--)
    {
       if ((*(*this)[i]) == data)
          break;
    }

    if (0 <= i)
       return((*this)[i]);
    else
       return(NULL);
}




/************* END OF FILE **************************************************/
