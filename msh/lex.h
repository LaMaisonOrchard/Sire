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

CLASS
   Tlex
   
DESC
   This is the generic lexical analysis class.
   
FILE
   lex.h

*****************************************************************************/


#ifndef LEX
#define LEX


#include <ctype.h>
#include <string.h>

#if defined(INTEL_NT) || defined (INTEL_9X)
typedef int DIR;
#else
#include <dirent.h>
#endif

#include "filestk.h"
#include "tokenstk.h"
#include "token.h"
#include "env.h"

class Tlex
{
public:

   Tlex (FILE *fp, Tenv *env = NULL);
   Tlex (text  ip, Tenv *env = NULL);
   Tlex (char const *nm, Tenv *env = NULL);
   
   virtual ~Tlex();

   virtual Ttoken *getToken () = 0;
   void            putBackToken (Ttoken *token);
                  
   Tenv           *getEnv();
   
   void            expand(char const *item, DIR *dir = NULL);
   void            pipe  (char const *comm);
   
protected:

   void        substitute(char const *var);
   Ttoken     *popToken();
   TfileStk    Dinput;

private:

   TtokenStk   DtokenStack;
   Tenv       *Denv;
};

inline   Tlex::Tlex (FILE *fp, Tenv *env)       : Dinput(fp), Denv(env) {}
inline   Tlex::Tlex (text  ip, Tenv *env)       : Dinput(ip), Denv(env) {}
inline   Tlex::Tlex (char const *nm, Tenv *env) : Dinput(nm), Denv(env) {}

inline   void Tlex::pipe (char const *comm)
{
   Dinput.push(new Tpipe(comm, TRUE, Denv));
}

inline   Tenv *Tlex::getEnv ()
{
   if (NULL == Denv)
      return(&(Tenv::appEnv()));
   else
      return(Denv);
}



#endif
