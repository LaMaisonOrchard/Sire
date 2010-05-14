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

   File  : class.cpp
   Class : Tclass
   Desc  : A description
       
EDIT 001 16-Jan-97 davido
Support TOUCH rules.
       
EDIT 002 29-Apr-97 davido
Support CREATE rules.

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nametarget.h"
#include "matchtarget.h"
#include "rule.h"
#include "text.h"
#include "sys.h"
#include "state.h"

Tlist Ttarget::Dlist;
Tlist TnameTarget::Dlist;
Tlist TmatchTarget::Dlist;

/************* FUNCTIONS ****************************************************/






int Ttarget::date (char const *name)
{
   // Take a local copy on the name
   Tstr     fName  = name;
   name = (char const*)fName;
   
   return(Drule->evaluate(name, target()));
}





int Ttarget::buildable (char const *name)
{
   // Take a local copy on the name
   Tstr     fName  = name;
   name = (char const*)fName;
   
   return(Drule->buildable(name, target()));
}







Ttarget::Ttarget (char const *target, Trule *rule) : Dtarget(target),Drule(rule)
{
   Dlist.add(this);
}

Ttarget::~Ttarget()
{
   for (int i = 0; (i < Dlist.len()); i++)
      if (Dlist[i] == this)
         {Dlist.remove(i); i--;}
}

int Ttarget::match (char const *name)
{
    if ((NULL == Drule) || removeS() ||
        ((!Drule->touchOnly()) && (!Drule->createOnly())))
    {
        return (TRUE);
    }
    else if (Drule->touchOnly())
    {
        if (removeS())
        {
            // We are removing redundant files
            return(FALSE);
        }
        else
        {
            // We can only touch a file that exists
            return(NO_FILE != ::timeStamp(name));
        }
    }
    else if (Drule->createOnly())
    {
        if (removeS())
        {
            // We are removing redundant files
            return(TRUE);
        }
        else
        {
            // We can only create a file that does not exist
            return(NO_FILE == ::timeStamp(name));
        }
    }
    else
    {
        return (TRUE);
    }
}

int Ttarget::find (char const *name, int start)
{
    int i;
    for (i = start; (i < Dlist.len()); i++)
        if (((Ttarget*)Dlist[i])->match(name))
            break;
         
    return(i);
}






TnameTarget::TnameTarget (char const *name, Trule *rule) : Ttarget(name, rule)
{
   Dlist.add(this);
}

TnameTarget::~TnameTarget()
{
   for (int i = 0; (i < Dlist.len()); i++)
      if (Dlist[i] == this)
         {Dlist.remove(i); i--;}
}

int TnameTarget::match (char const *name)
{
   return(Ttarget::match(name) && !strcmp(target(), name));  // 001
}

int TnameTarget::find (char const *name, int start)
{
    int i;
    for (i = start; (i < Dlist.len()); i++)
        if ((NULL != ((Ttarget*)Dlist[i])->rule()) && ((Ttarget*)Dlist[i])->match(name))
            break;
         
    return(i);
}






TmatchTarget::TmatchTarget (char const *target, Trule *rule) : Ttarget(target, rule)
{
   Dlist.add(this);
}

TmatchTarget::~TmatchTarget()
{
   for (int i = 0; (i < Dlist.len()); i++)
      if (Dlist[i] == this)
         {Dlist.remove(i); i--;}
}

int TmatchTarget::match (char const *name)
{
   return(Ttarget::match(name) && wildMatch(name, target()));   // 001
}

int TmatchTarget::find (char const *name, int start)
{
    int i;
    for (i = start; (i < Dlist.len()); i++)
        if ((NULL != ((Ttarget*)Dlist[i])->rule()) && ((Ttarget*)Dlist[i])->match(name))
            break;
         
    return(i);
}



/************* END OF FILE **************************************************/
