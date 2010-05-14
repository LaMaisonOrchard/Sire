/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2000, 2002, 2005  David W Orchard (davido@errol.org.uk)

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

   File  : rule.cpp
   Class : Trule
   Desc  : A description

EDIT 001 18-Apr-96 davido
Support for 'which' expansions.
       
EDIT 002 16-Jan-97 davido
Support TOUCH rules.
       
EDIT 003 29-Apr-97 davido
Support CREATE rules.
       
EDIT 004 12-Jul-99 davido
Support for indexed vaiables.
       
EDIT 005 07-Sep-99 davido
Implement late substitution for blocks.
       
EDIT 006 05-May-00 davido
Only actually do the build if that is what we are trying to do.

EDIT 007 06-Aug-02 davido@errol.org.uk
Add in modifier USER to specify files not to be deleted.

EDIT 008 14-Oct-02 davido@errol.org.uk
Clean up code for late variable resolution.

EDIT 009 14-Oct-02 davido@errol.org.uk
Implement USER properly

EDIT 010 05-Dec-02 davido@errol.org.uk
Get the correct date file for rules with no build script

EDIT 011 07-Jan-05 davido@errol.org.uk
Add some of the 'make' type build options.

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "strbuf.h"
#include "token.h"
#include "rule.h"
#include "text.h"
#include "identdep.h"
#include "calldep.h"
#include "whichdep.h"
#include "matchdep.h"
#include "state.h"
#include "sys.h"
#include "env.h"
#include "target.h"
#include "redundant.h"
#include "jlex.h"


/************* FUNCTIONS ****************************************************/


int Trule::DtryAll      = FALSE;
int Trule::DcheckBuild  = TRUE;
int Trule::DtouchOnly   = FALSE;


int Trule::ignoreErr (int state) {DcheckBuild = !state; return(0);}
int Trule::complete  (int state) {DtryAll     = state;  return(0);}
int Trule::touchFiles(int state) {DtouchOnly  = state;  return(0);}

Trule::Trule(TtokList &list, char const *block, ruleStates mode)
      : Dblock(block), Dposn(""),
        Dt(UNDEF), Dd(UNDEF), De(UNDEF), Donce(UNDEF),
        Duser(FALSE), Dtouch(FALSE), Dcreate(FALSE)
{
   Dbreak  = mode.breakS;
   Dcont   = mode.cont;
   if (mode.once)   Donce = SET;
   if (mode.recure) Donce = UNSET;
   
   if (mode.quiet)
   {
      Dt = FALSE;
      Dd = FALSE;
   }
   
   if (mode.talk)
   {
      Dt = TRUE;
   }
   
   if (mode.debug)
   {
      Dd = TRUE;
   }
   
   if (mode.touch)
   {
      Dtouch = TRUE;
   }
   else
   {
      Dtouch = FALSE;
   }
   
   if (mode.create)    // 003
   {
      Dcreate = TRUE;
   }
   else
   {
      Dcreate = FALSE;
   }
   
   if (mode.user)    // 007
   {
      Duser = TRUE;
   }
   else
   {
      Duser = FALSE;
   }
   
   for (int i = 0; (i < list.len()); i++)
   {
      Ttoken *token = list[i];
      
      switch (token->type())
      {
         case TOK_TEXT  :
         case TOK_IDENT :
            Ddeps.add(new TidentDep(token->text()));
            break;
            
         case TOK_CALL  :
            Ddeps.add(new TcallDep(token->text()));
            break;
            
         case TOK_WHICH :           // 001
            Ddeps.add(new TwhichDep(token->text()));
            break;
            
         case TOK_MATCH :
            Ddeps.add(new TmatchDep(token->text()));
            break;
            
         default :
            printE("Illegal token '%s' in depenents list\n", token->text());
      }
   }
}


Trule::~Trule()
{
}



TsubComm::TsubComm(TtokList &list, char const *block, ruleStates mode)
         : Trule(list, block, mode)
{
}


TsubComm::~TsubComm()
{
}


   
void Trule::realiseDeps (char const *target,
                         char const *targetMatch,
                         TstrBuf    *list,
                         TstrBuf    *real,
                         Tstr       *wildMatchList,  // 005
                         int         wildMatchlen)   // 005
{
   char const     *dep;
   
   unsigned long listPos = 0;
   unsigned long realPos = 0;
   unsigned long len     = 0;
   
   for (int i = 0; (i < Ddeps.len()); i++)
   {
      dep = Ddeps[i]->realise(target, targetMatch, wildMatchList, wildMatchlen);
      len = strlen(dep);
      
      list->cat(dep, listPos);
      listPos += len;
      (*list)[listPos++] = ' ';
      
      if (strcmp(NONE_DEP, dep)  &&
          strcmp(DUMMY_DEP, dep) &&
          strcmp(FORCE_DEP, dep))
      {
         real->cat(dep, realPos);
         realPos += len;
         (*real)[realPos++] = ' ';
      }
   }
   
   (*list)[listPos++] = '\0';
   (*real)[realPos++] = '\0';
}



char const *Trule::readVar(char const *&in,           // 008
                           Tstr       *list,
                           int         maxIdx) const
{
   TstrBuf buff;
   
   int           ch    = *(in++);
   unsigned long pos   = 0;
   
   if ('(' == ch)
   {
      ch = *(in++);
      while (Tjlex::isIdent(ch) || (SEP_S_VAR == ch)) // 009
      {
         if (SEP_S_VAR == ch)                  // 009
         {
             // Recursivly expand the variable
             
             char const *p = readVar(in, list, maxIdx);
             buff.cat(p, pos);
             pos += strlen(p);
         }
         else
         {
             buff[pos++] = ch;
         }
         ch = *(in++);
      }
      buff[pos++] = '\0';
      
      if (')' != ch)
      {
         // Bad variable name
         printE("Unterminated variable in block\n");
         in--;
      }
      else if (('0' < buff[0]) && (buff[0] <= '9') && ('\0' == buff[1]))
      {
         // Substitute the text matching the wildcard
         int idx = (buff[0] - '1');
         
         if (idx < maxIdx)
         {
            return(list[idx]);
         }
      }
      else
      {
         // Substitute the environment variable value
         char const *p = Tenv::appGet(buff);
         return((NULL == p)?(""):(p));
      }
   }
   else
   {
      // Bad variable name
      in--;
      return(SEP_S_VAR_STR);
   }
   
   return("");
}



   
char const *Trule::realiseBlock(char  const *target,
                                char  const *dependents,
                                Tstr *list,          // 005
                                int   maxIdx) const  // 005
{
   static TstrBuf rtn = "";
   
   char const    *p      = Dblock;
   unsigned long  rtnPos = 0;
   
   // Get the first dependent
   TstrBuf        firstDep;
   unsigned long  firstDepPos = 0;
   char const    *q           = dependents;
   
   while (isspace(*q)) q++;
   while (('\0' != *q) && !isspace(*q)) firstDep[firstDepPos++] = *(q++);
   
   firstDep[firstDepPos++] = '\0';
   
   // Substitute in the default variables where marked
   
   while ('\0' != *p)
   {
      if (SEP_S_VAR == *p)  // 005
      {
          // Expand the variable // 008
          
          p++;
          char const *var = readVar(p, list, maxIdx);
                                        
          rtn.cat(var, rtnPos);
          rtnPos += strlen(var);
      }
      else
      {
         // Copy the charactor
         rtn[rtnPos++] = *p++;
      }
   }
   
   rtn[rtnPos++] = '\0';
   
   return((char const*)rtn);
}



   
int Trule::evaluate(char const *name, char const *target)
{
   static  int   indent = 0;
   TstrBuf       dep = "";
   unsigned long depPos = 0;
   int           stamp  = NO_FILE;
   int           tstmp  = IRELEVENT;

   int finalRtn   = NO_FILE;
   
   Tstr    depName = "";
   
   // Take a local copy on the name
   Tstr fName  = name;
   name        = (char const*)fName;
   
   // Take a local copy on the target
   Tstr ftarget  = target;
   target        = (char const*)ftarget;
   
   // Generate a list of wildcard matches // 005
   Tstr    wildMatchList[10];
   int     wildMatchLen;
   
   wildMatchLen = wildMatchSections (name, target, wildMatchList);
   
   // Working space
   TstrBuf deps;
   TstrBuf realDeps; // deps excluding special dependents (ie just the real files)
   char const *p;
   
   printD("%s%s Attempt target %s for file %s\n", strindent(indent), posn(), target, name);
   indent++;
   
   realiseDeps (name, target, &deps, &realDeps, wildMatchList, wildMatchLen);  // 005
   p = (char const*)deps;
   
   while (isspace(*p)) p++;
   
   while ('\0' != *p)
   {
      // For each dependent
      
      // Read the dependent name
      
      depPos = 0;
      while (('\0' != *p) && !isspace(*p))
      {
         dep[depPos++] = *(p++);
      }
         
      dep[depPos++] = '\0';
   
      printD("%s%s Dependent is %s\n",
             strindent(indent), posn(), (char const*)dep);
      
      // Get the time stamp for the dependent
      
      if ((UNDEF == Donce)?(oneS()):(SET == Donce))
         tstmp = ::timeStamp((char const*)dep);
      else if (recursive((char const*)dep))
         tstmp = OLD_FILE;    // DUMMY dependent
      else
         tstmp = ::date((char const*)dep);
   
      printD("%s%s Date of %s is %d\n",
             strindent(indent), posn(), (char const*)dep, tstmp);
 
      
      if (BAD_FILE == tstmp)
      {
         printT("Failed for target %s dependant %s\n", target, (char const*)dep);
         indent--;

         finalRtn = BAD_FILE;
         if (!DtryAll) return(finalRtn);
      }
      else if (NO_FILE == tstmp)
      {
         // Bad target
         
         printD("%s%s This rule cannot be applied\n", strindent(indent), posn());
         indent--;
         return(IRELEVENT);
      }
      
      // Record the latest date
      if (tstmp > stamp)
      {
         depName = dep;
         stamp = tstmp;
      }
      
      // Remove white space
      while (isspace(*p)) {p++;}
   }

   if (finalRtn != NO_FILE) return(finalRtn);
   
   if (0 == depName.len())
   {
       printD("%s%s Rule has no dependents\n",
              strindent(indent), posn());
   }
   else
   {
       printD("%s%s Most recent dependent is %s (time stamp %d)\n",
              strindent(indent), posn(), (char const*)depName, stamp);
   }
   
   if ((NO_FILE == stamp) || (stamp >= timeStamp(name)) || allS())
   {
        if (Dblock.len() == 0)  // 000
        {
            // No build script
            // So the age of the file is the age of the youngest dependent
            // or the file if younger
            
            tstmp = timeStamp(name);
            
            if (tstmp > stamp) stamp = tstmp;
        }
        else if ((0 != execute(name, (char const*)realDeps, wildMatchList, wildMatchLen)) && DcheckBuild)  // 005
        {
            stamp = BAD_FILE;
        }
        else if (doRunS())  // 006
        {
            stamp = timeStamp(name);
        }
        else
        {
            stamp = NEW_FILE;  // 006
        }
   }
   else
   {
      // Not a break dependent
      stamp = timeStamp(name);
   }
   
   printD("%s%s Date of target %s = %d\n", strindent(indent), posn(), name, stamp);
   indent--;
   
   return(stamp);
}




   
int TsubComm::buildable(char const *name, char const *target)
{
    int stamp = Trule::buildable(name, target);
    
    // We can rebuild this
    if (stamp > OLD_FILE)
    {
        // Generate a list of wildcard matches
        
        Tstr    wildMatchList[10];
        int     wildMatchLen;

        wildMatchLen = wildMatchSections (name, target, wildMatchList);
        
        // Generate a list of dependents
        
        TstrBuf deps;
        TstrBuf realDeps; // deps excluding special dependents (ie just the real files)

        realiseDeps (name, target, &deps, &realDeps, wildMatchList, wildMatchLen); 

        // Execute the block with the '-remove' switch   
         
        Tenv::appSetStd(name, realDeps);
        TstrBuf block = Tenv::appGet("SIRE");
        block.cat(" -remove ");
        block.cat(realiseBlock(name, realDeps, wildMatchList, wildMatchLen));

        printT("%s\n", (char const*)block);

        fflush(stdout);

        if (doRunS() && (0 != ::execute((char const*)block)))
        {
            printE("Failed to remove files using the sub-command\n");
        }        

        Tenv::appUnsetStd();
    }
            
    return (stamp);
}




   
int Trule::buildable(char const *name, char const *target)
{
   static  int   indent = 0;
   TstrBuf       dep = "";
   unsigned long depPos = 0;
   int           stamp  = NO_FILE;
   int           tstmp  = IRELEVENT;
   
   // If this is a touch rule then we cannot use it.
   
   printD("%s%s Attempt target %s for file %s\n", strindent(indent), posn(), target, name);
   indent++;
   
   Tstr    depName = "";
   
   // Take a local copy on the name
   Tstr fName  = name;
   name        = (char const*)fName;
   
   // Take a local copy on the target
   Tstr ftarget  = target;
   target        = (char const*)ftarget;
   
   // Generate a list of wildcard matches // 005
   Tstr    wildMatchList[10];
   int     wildMatchLen;
   
   wildMatchLen = wildMatchSections (name, target, wildMatchList);
   
   TstrBuf deps;
   TstrBuf realDeps;
   char const *p;
   
   realiseDeps (name, target, &deps, &realDeps, wildMatchList, wildMatchLen);  // 005
   p = (char const*)deps;
   
   while (isspace(*p)) p++;
   
   while ('\0' != *p)
   {
      // For each dependent
      
      // Read the dependent name
      
      depPos = 0;
      while (('\0' != *p) && !isspace(*p))
      {
         dep[depPos++] = *(p++);
      }
         
      dep[depPos++] = '\0';
   
      printD("%s%s Dependent is %s\n",
             strindent(indent), posn(), (char const*)dep);
      
      // Get the time stamp for the dependent
      
      if ((UNDEF == Donce)?(oneS()):(SET == Donce))
         tstmp = ::timeStamp((char const*)dep);
      else if (recursive((char const*)dep))
         tstmp = OLD_FILE;    // DUMMY dependent
      else
         tstmp = ::buildable((char const*)dep);
   
      printD("%s%s Date of %s is %d\n",
             strindent(indent), posn(), (char const*)dep, tstmp);
 
      
      if (BAD_FILE == tstmp)
      {
         // This should never happen since we have no builds to fail
         printT("Failed for target %s dependant %s\n", target, (char const*)dep);
         indent--;
         return(BAD_FILE);
      }
      else if (NO_FILE == tstmp)
      {
         // Bad target
         
         printD("%s%s This rule cannot be applied\n", strindent(indent), posn());
         indent--;
         return(IRELEVENT);
      }
      
      // Record the latest date
      if (tstmp > stamp)
      {
         depName = dep;
         stamp = tstmp;
      }
      
      // Remove white space
      while (isspace(*p)) {p++;}
   }
   
   if (0 == depName.len())
   {
       printD("%s%s Rule has no dependents\n",
              strindent(indent), posn());
       
       stamp = NEW_FILE;
   }
   else
   {
       printD("%s%s Most recent dependent is %s (time stamp %d)\n",
              strindent(indent), posn(), (char const*)depName, stamp);
   }
   
   // Can't delete it is it is user editable
   
   if (touchOnly())
   {
        printD("%s%s TOUCH only rules %s for file %s\n", strindent(indent), posn(), target, name);
        return(IRELEVENT);
   }
   else if (user())  // 009
   {
        printD("%s%s This rule applies but the file is user editable\n", strindent(indent), posn());
        return(IRELEVENT);
   }

   return(stamp);
}


int Trule::execute(char  const *name,
                   char  const *deps,
                   Tstr *wildMatchList,      // 005
                   int   wildMatchLen) const // 005
{
    int rtn = 0;

    if (DtouchOnly)
    {
        // Touch the file
        touch(name);
    }
    else
    {
        Tenv::appSetStd(name, deps);
        char const *block = realiseBlock(name, deps, wildMatchList, wildMatchLen);

        if ('\n' != block[strlen(block) -1])
           printT("%s\n", block);
        else
           printT("%s", block);

        fflush(stdout);

        if (doRunS()) rtn = ::execute(block);  // 006

        Tenv::appUnsetStd();
    }
    
    return(rtn);
}


int TsubComm::execute(char  const *name,
                   char  const *deps,
                   Tstr *wildMatchList,      // 005
                   int   wildMatchLen) const // 005
{
    int rtn = 0;
    
    
    Tenv::appSetStd(name, deps);
    TstrBuf block = Tenv::appGet("SIRE");
    block.cat(" ");
    block.cat(realiseBlock(name, deps, wildMatchList, wildMatchLen));
   
    printT("%s\n", (char const*)block);
    
    fflush(stdout);
             
    if (doRunS()) rtn = ::execute((char const*)block);
       
    Tenv::appUnsetStd();
    
    return(rtn);
}



int Trule::execute(char  const *name,
                   char  const *deps) const
{
      // There is no wildcard matching so just pass an empty list
      Tstr wildMatchList[1];
      int  wildMatchLen = 0;
      
      return(execute(name, deps, wildMatchList, wildMatchLen));
}



int Trule::source(char const *name, char const *deps) const
{
      // There is no wildcard matching so just pass an empty list
      Tstr wildMatchList[1];
      int  wildMatchLen = 0;
      
      Tenv::appSetStd(name, deps);
      char const *block = realiseBlock(name, deps, wildMatchList, wildMatchLen);
   
      if ('\n' != block[strlen(block) -1])
         printT("%s\n", block);
      else
         printT("%s", block);
      
      fflush(stdout);
               
      int rtn = ::source(block);
         
      Tenv::appUnsetStd();
      
      return(rtn);
}



void Trule::printT(char const *format ...) const
{
   va_list ap;
   va_start(ap, format);
   int state;
   
   if (UNDEF != Dt) state = ::setTon(Dt);
    
   vprintT(format, ap);
   
   if (UNDEF != Dt) ::setTon(state);
    
   va_end(ap);
}



void Trule::printD(char const *format ...) const
{
   va_list ap;
   va_start(ap, format);
   int state;
   
   if (UNDEF != Dd) state = ::setDon(Dd);
    
   vprintD(format, ap);
   
   if (UNDEF != Dd) ::setDon(state);
    
   va_end(ap);
}



void Trule::printE(char const *format ...) const
{
   va_list ap;
   va_start(ap, format);
   int state;
   
   if (UNDEF != De) state = ::setEon(De);
    
   vprintE(format, ap);
   
   if (UNDEF != De) ::setEon(state);
    
   va_end(ap);
}


ruleStates::ruleStates(int i)
{
   user   = i;    // 007
   breakS = i;
   cont   = i;
   quiet  = i;
   talk   = i;
   debug  = i;
   once   = i;
   recure = i;
   touch  = i;    // 002
   create = i;    // 002
}



/************* END OF FILE **************************************************/
