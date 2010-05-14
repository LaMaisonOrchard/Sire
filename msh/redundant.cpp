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

   File  : redundant.cpp
   Desc  : A description

EDIT 001 14-Nov-02 davido
Implement USER properly

EDIT 002 18-Nov-02 davido
Make SIRE_PARTIAL consistent with other state variables.

*****************************************************************************/

#include <stdlib.h>

#include "str.h"
#include "sys.h"
#include "text.h"
#include "env.h"
#include "state.h"
#include "target.h"
#include "nametarget.h"
#include "matchtarget.h"
#include "targetstamp.h"
#include "targetstmplst.h"
#include "rule.h"
#include "redundant.h"








// Check and remove this file
int redundant(char const *fullName)
{
    static TtargetStmpLst targetList;
   
    TtargetStamp *targetRef;
   
    int      stamp  = IRELEVENT;
    int      pos    = 0;
    Ttarget *target = NULL;
         
    // Check for the DUMMY dependent
    if (!strcmp(fullName, NONE_DEP))  return(NO_FILE);
    if (!strcmp(fullName, DUMMY_DEP)) return(OLD_FILE);
    if (!strcmp(fullName, FORCE_DEP)) return(NEW_FILE);
   
    // Clean up the target name
    Tstr	    buffer = cleanName(fullName);
    char const *name   = (char const*)buffer;
 
    // Check for an old target
    if (NULL != (targetRef = targetList.find(name)))
    {
       printD("Already done '%s'\n", name);
       return(targetRef->stamp());
    }
 
    // Can we rebuild this file
    // ie can we find a rule for this where all the dependents are buildable

    if (TnameTarget::len() > (pos = TnameTarget::find(name, pos)))
    {
       // Explicite target

       target = TnameTarget::get(pos);
       
       stamp  = target->buildable(name);
 
       if (NO_FILE == stamp) stamp = NEW_FILE; // No dependents
    }
    else
    {
       // Posible Implicite target
 
       pos   = 0;
       stamp = IRELEVENT;
       while (TmatchTarget::len() > (pos = TmatchTarget::find(name, pos)))
       {
    	  target  = TmatchTarget::get(pos);
       
          int stp = target->buildable(name);
    	  if (IRELEVENT != stp)
    	  {
    	     stamp = stp;
    	     break;
    	  }
    	  if (target->rule()->breakHere()) break;
          
    	  pos++;
       }
    }
    
    if (stamp > OLD_FILE)
    {
        // We can rebuild this
        int actual = timeStamp(name); // All rules fail
	
	    if (actual > stamp)
	    {
	       stamp = actual; 
	    }
	
        if (!access((char*)name, F_OK))
        {
	       printT("unlink [%s]\n", name);
           if (doRunS()) unlink(name);  // 000
        }
    }
   
    // Add the target name with time stamp to the list
    if (NULL != (targetRef = targetList.find(name)))
       targetList.del(targetRef);
 
    targetList.add(new TtargetStamp(name, stamp));

    return (stamp);
}


// Can we build this file
// It is redundant (ie we can remake it from other files
// or the file exists.

int buildable(char const *name)
{
    int         stamp = timeStamp(name);
    char const *p;
    
    // Check for partial targets
    if ((NO_FILE < stamp) 	               &&
    	(NULL != (p = Tenv::appGet("SIRE_PARTIAL"))) &&
        (*p == 't') &&
    	partialTarget(name))
    {
       printD("Partial target '%s'\n", name);
       return (stamp);
    }
   
    stamp = redundant(name);
    
    if (stamp <= OLD_FILE) stamp = timeStamp(name);
    
    return (stamp);   
}




