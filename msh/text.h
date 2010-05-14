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

   File  : text.cpp
   Class : NONE
   Desc  : A description

*****************************************************************************/



/************* INCLUDE ******************************************************/



/************* FUNCTIONS ****************************************************/

#include "str.h"

char const *sedMatch(char const *text,
                     char const *firstMatch,
                     char const *secondMatch);

int wildMatchSections (char const *name,
                       char const *match,
                       Tstr        matchList[]);
                       
int wildMatch (char const *name, char const *match);

char const *markMatch (char const *match);
                    
int date      (char const *name);
int recursive (char const *dep);

char const *strindent(int indent);
char const *strindent(int indent);

char const *cleanName(char const *);

int partialTarget(char const *target);

#define NONE_DEP  "NONE"
#define DUMMY_DEP "DUMMY"
#define FORCE_DEP "FORCE"





/************* END OF FILE **************************************************/















