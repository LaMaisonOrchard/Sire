/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 2008  David W Orchard (davido@errol.org.uk)

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

   File  : msh.h
   Class : none
   Desc  : A description

*****************************************************************************/


#ifndef MSH
#define MSH

#include "mlex.h"

int msh(char const *script, char const **argv, char **envp);


#include "mlex.h"
#include "conditional.h"
#include "jake.h"
#include "toklist.h"



class Tmsh : public Tconditional
{
public:

    Tmsh (FILE *fp = NULL);
    Tmsh (text nm);
    
    int state() const {return DlastRtn;}
   
protected:

private:
    int  parser();
    
    void strip(Ttoken *token);
    
    parseState_t parseStatements(int active, int restricted);
    virtual Tlex        *newLex            (char const *file);
    virtual Tlex        *newLex            (FILE       *file);

    int       DlastRtn;
};


inline Tmsh::Tmsh (FILE *fp)  : Tconditional(new Tmlex(fp))  {parser();}
inline Tmsh::Tmsh (text txt)  : Tconditional(new Tmlex(txt)) {parser();}



#endif
