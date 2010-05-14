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

   File  : pipe.h
   Class : Tpipe
   Desc  : A description

*****************************************************************************/


#ifndef PIPE
#define PIPE


#include <stdio.h>
#include "str.h"

class Tenv;
   
char const *makeScript(char const *text, int source = FALSE);


class Tpipe
{
public:

   Tpipe(char const *script, int read = TRUE, Tenv *env = NULL);
   Tpipe(FILE *fd, int pid, char const *script);
   ~Tpipe();
   
   FILE *file()  {return(Dfd);}
   int   state() {return(Dstate);}

   static Tpipe *find(FILE *fd);
   static Tpipe *find(int pid);
   
   static void clear();
   
protected:

private:

   Tpipe *Dnext;
   FILE  *Dfd;
   int    Dpid;
   Tstr   Dscript;
   
   static Tpipe *Dhead;
   static int    Dstate;
   static int    DsetClear;

};


#endif
