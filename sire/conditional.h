/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2002, 2003  David W Orchard (davido@errol.org.uk)

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

   File  : conditional.h
   Class : Tconditional
   Desc  : A description
   
AMENDMENTS

EDIT 001 14-Sept-08 david@errol.org.uk
First version based on jake.cpp

*****************************************************************************/


#ifndef CONDITIONAL
#define CONDITIONAL


#include "jlex.h"
#include "toklist.h"
#include "rule.h"


enum parseState_t
{
    FILE_END = 0,
    FILE_EXIT,
    PARSE_ELSE,
    PARSE_ELSE_IF,
    PARSE_END_IF
};



class Tconditional
{
public:

    int parseFile(char const *file, int active = TRUE, Ttoken **list = NULL, int len = 0);
   
protected:

    Tconditional (Tlex *lex = NULL);

    virtual parseState_t parseStatements   (int active, int restricted) = 0;
    virtual Tlex        *newLex            (char const *file) = 0;
    virtual Tlex        *newLex            (FILE       *file) = 0;

    int parseIf           (int active, int restricted);
    int parseElse         (int active);
    int parseElseIf       (int active);
    int parseEndIf        (int active);
    int parseCondition    (int &active);

    int parseComparisonExpression(int &active);
    int parseComparisonCondition(int &active);

    void pushInput  (Tlex*, Ttoken **list = NULL, int len = 0);
    void popInput   ();

    char const *expandTokenText(Ttoken *token);

    Tlex      *Dinput;
    
    bool doExit()  {return Dexit;}
    void setExit() {Dexit = true;}

private:

    struct
    {
        Tlex    *input;
        Ttoken **argList;
        int      argLen;
    }
    DinputStack[64];

    int        DinputStackPos;
    bool       Dexit;
};

inline   Tconditional::Tconditional (Tlex *lex)       : DinputStackPos(0), Dinput(lex), Dexit(FALSE) {}


#endif
