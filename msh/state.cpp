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

   File  : state.cpp
   Class : None
   Desc  : A description

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "state.h"
#include "text.h"
#include "sys.h"
#include "env.h"


static int doRun      = 1;  // Actually run the build commands
static int talk       = 1;
static int debug      = 0;
static int one        = 0;
static int all        = 0;  // Re build everything
static int removeFlag = 0;

/************* FUNCTIONS ****************************************************/








int talkS()   {return(talk);}
int debugS()  {return(debug);}
int oneS()    {return(one);}
int removeS() {return(removeFlag);}
int allS()    {return(all);}
int doRunS()  {return(doRun);}

int talkS  (int s) {int t = talk;  talk  = s; Tenv::appSet("TALK_STATE", (s)?("t"):("f"));  setTon(s); return(t);}
int debugS (int s) {int t = debug; debug = s; Tenv::appSet("DEBUG_STATE", (s)?("t"):("f")); setDon(s); return(t);}
int quietS (int s) {int t = talkS(!s) || debugS(!s); return(t);}

int removeS(int s) {int t = removeFlag; removeFlag = s; Tenv::appSet("REMOVE_STATE", (s)?("t"):("f")); return(t);}
int allS   (int s) {int t = all; all = s; return(t);}
int doRunS (int s) {int t = doRun; doRun = s; return(t);}


int oneS   (int s) {int t = one;  one  = s; Tenv::appSet("ONE_STATE", (s)?("t"):("f"));  return(t);}


/************* END OF FILE **************************************************/
