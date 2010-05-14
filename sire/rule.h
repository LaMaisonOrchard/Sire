/****************************************************************************


    The Sire build utility 'sire'.
    Copyright (C) 1999, 2002, 2003, 2005  David W Orchard (davido@errol.org.uk)

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

   File  : rule.h
   Class : Trule
   Desc  : A description
       
EDIT 001 16-Jan-97 davido
Support TOUCH rules.
       
EDIT 002 29-Apr-97 davido
Support CREATE rules.

EDIT 003 06-Aug-02 davido@errol.org.uk
Add in modifier USER to specify files not to be deleted.

EDIT 004 14-Oct-02 davido@errol.org.uk
Support for late variable resolution.

EDIT 005 19-Oct-03 davido@errol.org.uk
Add support for sub-commands. The sub-command class is derived
from the rule class and provides a specialisation.

EDIT 006 07-Jan-05 davido@errol.org.uk
Add options to control how all rules are built.

*****************************************************************************/


#ifndef RULE
#define RULE


#include "str.h"
#include "toklist.h"
#include "deplist.h"

class TstrBuf;


struct ruleStates
{
   ruleStates(int i = FALSE);
   
   int user;      // Can be updated and created but not deleted // 003
   int breakS;    // Stop here even if the dependents don't match
   int cont;      // Contine and find other rules matching the target
   int quiet;     // No output
   int talk;      // Produce standard output
   int debug;     // Output debug information
   int once;      // Don't build the dependents
   int recure;    // Build the dependents
   int touch;     // Can be updated but not created // 001
   int create;    // Can be created but not updated // 002
};


class Trule
{
public:

   Trule (TtokList &list, char const *block, ruleStates mode);
   virtual ~Trule();
   
   void realiseDeps (char const *target,
                     char const *targetMatch,
                     TstrBuf    *list,
                     TstrBuf    *real,
                     Tstr       *wildMatchList,
                     int         wildMatchlen);
   
   char const *realiseBlock(char const *target,
                            char const *deps,
                            Tstr       *wildMatchList,
                            int         wildMatchlen) const;
   
   int         evaluate    (char const *name,
                            char const *target);
   
   virtual int buildable   (char const *name,
                            char const *target);
                            
   void        setPosn(char const *posn);
   char const *posn() const;
   
   int breakHere() const ;
   int continueTarget() const;
   
   int execute(char const *name,
               char const *deps) const;
   
   virtual int execute(char const *name,
                       char const *deps,
                       Tstr       *wildMatchList,
                       int         wildMatchLen) const;
               
   int source (char const *name, char const *deps) const;
   
   int user()       const;  // 003
   int touchOnly()  const;
   int createOnly() const;

void printT(char const *format ...) const;
void printD(char const *format ...) const;
void printE(char const *format ...) const;

static int ignoreErr (int state);
static int complete  (int state);
static int touchFiles(int state);

private:

    char const *readVar(char const *&in,       // 004
                        Tstr       *list,
                        int         maxIdx) const;

enum printState {UNDEF = -1, UNSET, SET};

   TdepList Ddeps;
   Tstr     Dblock;
   Tstr     Dposn;
   
   int Dt;
   int Dd;
   int De;
   int Donce;
   int Duser;    // 003
   int Dtouch;   // 001
   int Dcreate;  // 002
   
   // Modifiers
   
   int      Dbreak;
   int      Dcont;

   static int DtryAll;
   static int DcheckBuild;
   static int DtouchOnly;
};

class TsubComm : public Trule
{
public:

   TsubComm (TtokList &list, char const *block, ruleStates mode);
   virtual ~TsubComm();
   
   virtual int buildable   (char const *name,
                            char const *target);
   
   virtual int execute(char const *name,
                       char const *deps,
                       Tstr       *wildMatchList,
                       int         wildMatchLen) const;
};

inline void Trule::setPosn(char const *posn) {Dposn = posn;}
inline char const *Trule::posn() const {return((char const*)Dposn);}
inline int Trule::breakHere() const {return(Dbreak);}
inline int Trule::continueTarget() const {return(Dcont);}
   
inline int Trule::touchOnly()  const {return(Dtouch);}   // 001
inline int Trule::createOnly() const {return(Dcreate);}  // 002
inline int Trule::user()       const {return(Duser);}  // 003

#endif
