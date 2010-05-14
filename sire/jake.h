/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2002, 2003, 2008  David W Orchard (davido@errol.org.uk)

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

   File  : jake.h
   Class : Tjake
   Desc  : A description
   
AMENDMENTS

EDIT 001 17-Jun-98 davido
Support sourcing jake environment files

EDIT 002 15-Nov-02 davido@errol.org.uk
Support script arguments

*****************************************************************************/


#ifndef JAKE
#define JAKE


#include "jlex.h"
#include "conditional.h"
#include "toklist.h"
#include "rule.h"



class Tjake : public Tconditional
{
public:

    Tjake (FILE *fp = NULL);
    Tjake (char const *nm);
   
    void preamble();
    void postamble();
   
protected:

private:

    virtual parseState_t parseStatements   (int active, int restricted);
    virtual Tlex        *newLex            (char const *file);
    virtual Tlex        *newLex            (FILE       *file);

    int parsePreamble     (int active);
    int parsePostamble    (int active);
    int parsePartial      (int active);
    int parseState        (int active);
    int parseSource       (int active);
    int parseAssign       (int active);     // 001
    int parseRule         (int active);
    int parseRuleNmLst    (TtokList &list);
    int parseRuleModifiers(ruleStates &mode);
    int parseRuleDepsLst  (TtokList &list);

    int parseComparisonExpression(int &active);
    int parseComparisonCondition(int &active);

    void parser();

    Trule     *Dpreamble;
    Trule     *Dpostamble;
};

inline   Tjake::Tjake (FILE *fp)       : Tconditional(new Tjlex(fp)) {parser();}
inline   Tjake::Tjake (char const *nm) : Tconditional(new Tjlex(nm)), Dpreamble(NULL), Dpostamble(NULL) {parser();}

   
inline   void Tjake::preamble()  {if (NULL != Dpreamble)  Dpreamble ->source ("","");}
inline   void Tjake::postamble() {if (NULL != Dpostamble) Dpostamble->execute("","");}


#endif
