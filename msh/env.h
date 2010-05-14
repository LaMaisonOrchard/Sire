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

   File  : env.h
   Class : Tenv
   Desc  : A description
   
AMENDMENTS

EDIT 001 23-Jul-99 davido
Add a version of setApp() to read the environment from a file.

EDIT 002 15-Nov-99 davido@errol.org.uk
Add support for application arguments.

*****************************************************************************/


#ifndef ENV
#define ENV


#include "strbuf.h"

class Ttoken;
class Tstr;


class Tenv
{
public:

   Tenv ();
   Tenv (Tenv const &env);
   Tenv (char const * const *envp);
   Tenv (FILE *file);
   ~Tenv();
   
   // Load the application specific environment
   
   static void setApp  (char const * const *envp);
   static void setApp  (FILE *file);      /* 001 */

   // Generic environment methods   
   
   void setStd  (char const *target, char const *deps);
   void unsetStd();
   
   char       **getEnvp();
   void         set    (char const *name, char const *value);
   void         unset  (char const *name);
   char const  *get    (char const *name);
   char const  *expand (char const *name);    // Expand simple environment variable expressions
   void         clear  ();                    // Clear all environment variables
   
   // Application specific environment methods  
   
   static void appSetStd  (char const *target, char const *deps);
   static void appUnsetStd();
   
   static char       **appGetEnvp();
   static void         appSet    (char const *name, char const *value);
   static void         appUnset  (char const *name);
   static char const  *appGet    (char const *name);
   static char const  *appExpand (char const *name);    // Expand simple environment variable expressions
   static Tenv        &appEnv    ();
   static void         appClear  ();                    // Clear all environment variables
   static void         appSetArg (Ttoken **List, int len); // Set up the applications argument list
   
protected:

private:

   static Tenv  DappEnvp;   // Application specific environment
   static Tstr *DappList;
   static int   Dapplen;
   
   TstrBuf Dtarget;
   TstrBuf Dfirst;
   TstrBuf Ddeps;
   int     Dset;
   
   // Table of environment variables in the "<name>=<value>" format
   // malloc() is used to allocate space to be usable with exec() etc.

   char **Denvp;
   
};


   
inline void Tenv::appSetStd (char const *target, char const *deps)
{
   DappEnvp.setStd(target, deps);
}

inline void Tenv::appUnsetStd()
{
   DappEnvp.unsetStd();
}
   
inline char **Tenv::appGetEnvp()
{
   return(DappEnvp.getEnvp());
}
   
inline void Tenv::appSet (char const *name, char const *value)
{
   DappEnvp.set(name, value);
}

inline char const *Tenv::appGet (char const *name)  // 002
{
    return(DappEnvp.get(name));
}

inline void Tenv::appUnset (char const *name)
{
   DappEnvp.unset(name);
}

inline char const *Tenv::appExpand (char const *name)
{
   return(DappEnvp.expand(name));
}

inline Tenv &Tenv::appEnv ()
{
   return(DappEnvp);
}

inline void Tenv::appClear ()
{
   DappEnvp.clear();
}



#endif
