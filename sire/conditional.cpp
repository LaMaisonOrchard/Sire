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

   File  : conditional.cpp
   Class : Tconditional
   Desc  : A description

EDIT 001 14-Sept-08 david@errol.org.uk
First version based on jake.cpp

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "conditional.h"
#include "text.h"
#include "sys.h"
#include "env.h"
#include "str.h"



/************* FUNCTIONS ****************************************************/






int Tconditional::parseFile(char const *file, int active, Ttoken **list, int len)   // 07
{
	Tlex *lex = newLex(file);
    
    // Set up the argument list
    
    if ((NULL != list) && (len > 0))
    {
        char const **argList = new char const*[len];
        
        for (int i = 0; (i < len); i++)
        {
            argList[i] = list[i]->text();
        }
    }
	
    // Push the lex.
	
    pushInput(lex, list, len);
	
    // Read the lex file
	
    parseStatements(active, FALSE);

    // Pop the lex file

	popInput();
    
    // Restore the old argument list
    delete lex;

    return (0);
}


int Tconditional::parseIf(int active, int restricted)
{
    Ttoken       *token       = NULL;
    int           localActive = FALSE;
    parseState_t  rtn;
    Tstr          posn = "";

    token = Dinput->getToken();
    while ((NULL != token) && TOK_COMMENT == token->type())
    {
	    delete token;
	    token = Dinput->getToken();
    }

    // Check for a source statement
    if ((NULL != token) && (TOK_IF == token->type()))
    {
        posn = token->posn();
	    delete token;
	
        do
        {
            if (localActive) active = FALSE; // We have done one of the conditions
            if (!parseCondition(localActive))
            {
                // Illegal condition
                printE("%s : Bad condition\n", (char const*)posn);
            }
        }
        while(PARSE_ELSE_IF == (rtn = parseStatements((active && localActive), restricted)));

        if (PARSE_ELSE == rtn)
        {
            rtn = parseStatements((active && !localActive), restricted);
        }

        if ((PARSE_END_IF != rtn) && (FILE_EXIT != rtn))
        {
            printE("%s : IF Statement not terminated\n", (char const*)posn);
        }

        return(1);
    }
    else
    {
        // Not an IF statement
        Dinput->putBackToken(token);
    }

    return (0);
}



int Tconditional::parseElse(int active)
{
    Ttoken *token    = NULL;

    token = Dinput->getToken();
    while ((NULL != token) && (TOK_COMMENT == token->type()))
    {
	    delete token;
	    token = Dinput->getToken();
    }

    // Check for a source statement
    if ((NULL != token) && TOK_ELSE == token->type())
    {
	    delete token;
        return (1);
    }
    else
    {
        Dinput->putBackToken(token);
    }

    return (0);
}



int Tconditional::parseElseIf(int active)
{
    Ttoken *token    = NULL;

    token = Dinput->getToken();
    while ((NULL != token) && TOK_COMMENT == token->type())
    {
	    delete token;
	    token = Dinput->getToken();
    }

    // Check for a source statement
    if ((NULL != token) && (TOK_ELIF == token->type()))
    {
	    delete token;
        return (1);
    }
    else
    {
        Dinput->putBackToken(token);
    }

    return (0);
}



int Tconditional::parseEndIf(int active)
{
    Ttoken *token    = NULL;

    token = Dinput->getToken();
    while ((NULL != token) && TOK_COMMENT == token->type())
    {
	    delete token;
	    token = Dinput->getToken();
    }

    // Check for a source statement
    if ((NULL != token) && (TOK_ENDIF == token->type()))
    {
	    delete token;
        return (1);
    }
    else
    {
        Dinput->putBackToken(token);
    }

    return (0);
}



int Tconditional::parseCondition(int &active)
{
    int     ok        = FALSE;
    int     state     = 0;
    int     thisState = 0;

    ok = parseComparisonExpression(state);

    if (ok) active = state;

    return (ok);
}



int Tconditional::parseComparisonExpression(int &active)
{
    int     ok        = FALSE;
    int     openExpr  = FALSE;
    int     state     = 0;
    int     thisState = 0;

    ok = parseComparisonCondition(state);

    Ttoken *arg1 = Dinput->getToken();

    while (ok && (NULL != arg1) && ((TOK_AND == arg1->type()) || (TOK_OR == arg1->type())))
    {
        ok = parseComparisonExpression(thisState);

        switch (arg1->type())
        {
            case TOK_AND :
                state = state && thisState;
                break;

            case TOK_OR :
                state = state || thisState;
                break;

            default:
                printE("%s : Bad token in expression [%s]\n", arg1->posn(), arg1->text());
                ok = FALSE;
                break;
        }

        delete arg1;
        arg1 = Dinput->getToken();
    }

    if (!ok)
    {
            printE("Bad expression\n");
        // We had an error somewhere
        Dinput->putBackToken(arg1);
    }
    else
    {
        // Read beyond the end
        Dinput->putBackToken(arg1);
        active = state;
    }

    return (ok);
}



int Tconditional::parseComparisonCondition(int &active)
{
    int     ok       = FALSE;
    int     state    = FALSE;
    int     notState = FALSE;

    Ttoken *arg1 = Dinput->getToken();

    switch(arg1->type())
    {
        case TOK_NOT:
        {
            notState = 1;
            ok = parseComparisonCondition(state);
            break;
        }

        case TOK_OPEN:
        {
            ok = parseCondition(state);

            Ttoken *arg2 = Dinput->getToken();

            if (!ok)
            {
                Dinput->putBackToken(arg2);
            }
            else if (TOK_CLOSE != arg2->type())
            {  
                ok = FALSE;
                printE("%s : Missing close bracket\n", arg1->posn());
                Dinput->putBackToken(arg2);
            }
            else
            {
                delete arg2;
            }
            delete arg1;

            break;
        }

        case TOK_FILE:
        case TOK_DIR:
        case TOK_EXISTS:
        case TOK_EXECUTABLE:
        {
            Ttoken *arg2 = Dinput->getToken();

            if ((NULL != arg2) &&
	            ((TOK_TEXT  == arg2->type()) ||
                 (TOK_IDENT == arg2->type()) ||
                 (TOK_CALL  == arg2->type()))
	           )
            {
                ok = TRUE;
                char const *text = expandTokenText(arg2);

                switch(arg1->type())
                {
                    case TOK_FILE:       state = isFile(text); break;
                    case TOK_DIR:        state = isDir(text);  break;
                    case TOK_EXISTS:     state = doesExist(text); break;
                    case TOK_EXECUTABLE: state = isExec(text); break;
                    default: state = FALSE; break;
                }
            }
            else
            {  
                printE("%s : Bad file operation\n", arg1->posn());
                Dinput->putBackToken(arg2);
            }

            delete arg1;

            break;
        }

        case TOK_TEXT:
        case TOK_IDENT:
        case TOK_CALL:
        case TOK_WHICH:
        case TOK_MATCH:
        {
            Ttoken *op   = Dinput->getToken();
            Ttoken *arg2 = Dinput->getToken();

            if ((NULL != arg2) &&
	            ((TOK_TEXT  == arg2->type()) ||
                 (TOK_IDENT == arg2->type()) ||
                 (TOK_CALL  == arg2->type()) ||
                 (TOK_WHICH == arg2->type()) ||
                 (TOK_MATCH == arg2->type()))
	            )
            {
                char const *text1;
                char const *text2;
                
                switch (op->type())
                {
                    case TOK_NOT_EQUAL:
                        notState = 1;

                    case TOK_EQUAL:
                        ok = 1;

                        if ((TOK_MATCH == arg2->type()))
                        {
                            text1 = expandTokenText(arg1);
                            state = wildMatch(text1, arg2->text());
                        }
                        else if ((TOK_MATCH == arg1->type()))
                        {
                            text2 = expandTokenText(arg2);
                            state = wildMatch(text2, arg1->text());
                        }
                        else
                        {
                            text1 = expandTokenText(arg1);
                            text2 = expandTokenText(arg2);
                            state = !strcmp(text1, text2);
                        }

                        ok = TRUE;
                        break;

                    default:
                        break;
                }
            }

            if (!ok)
            {
                printE("Bad comparison\n");
                Dinput->putBackToken(arg2);
                Dinput->putBackToken(op);
            }
            else
            {
                delete arg2;
                delete op;
                delete arg1;
            }
            break;
        }

        default:
        {
            printE("Bad condition\n");
            Dinput->putBackToken(arg1);
            break;
        }
    }

    active = (notState)?(!state):(state);

    return(ok);
}





void Tconditional::pushInput(Tlex *input, Ttoken **list, int len)
{
    if (NULL == input)
    {
        printE("Internal error - Bad input source\n");
    }
    else if (DinputStackPos == sizeof(DinputStack)/sizeof(DinputStack[0]))
    {
        printE("Internal error - Input Stack overflow\n");
    }
    else
    {
        if (list == NULL)
        {
            // If the arguments are not defined use the existing arguments
            
            list = NULL;
            len  = 0;
        }
        else
        {
            // Set the new arg list
        
            Tenv::appSetArg(list, len);
        }
        
        DinputStack[DinputStackPos].argList = list;
        DinputStack[DinputStackPos].argLen  = len;
        
        DinputStack[DinputStackPos++].input   = Dinput;
        Dinput = input;
    }
}



void Tconditional::popInput()
{
    if (0 == DinputStackPos)
    {
        printE("Internal error - Input Stack underflow\n");
    }
    else
    {
        Dinput = DinputStack[--DinputStackPos].input;
        
        // Restore the argument list
        
        if (DinputStackPos > 0)
        {
            Tenv::appSetArg(DinputStack[DinputStackPos].argList,
                            DinputStack[DinputStackPos].argLen);
        }
        else
        {
            Tenv::appSetArg(NULL, 0);
        }
    }
}
      
      
char const *Tconditional::expandTokenText(Ttoken *token)
{
    switch (token->type())
    {
        case TOK_CALL  :
            {
               FILE          *fp  = readPipe(token->text());
               unsigned long  pos = 0;
               static TstrBuf buff;
             
               if (NULL != fp)
               {
                  int ch;
                
                  while (EOF != (ch = fgetc(fp)))
                  {
                     // Minimise the use of spaces
                     if (isspace(ch))
                     {
                        do
                        {
                            ch = fgetc(fp);
                        }
                        while (isspace(ch));
                        
                        if (EOF != ch)
                            buff[pos++] = ' ';
                     }
                   
                     if (EOF != ch)
                        buff[pos++] = ch;
                  }
                  
                  buff[pos++] = '\0';
                
                  closePipe(fp);
                  
                  return ((char const*)buff);
               }
               else
               {
                  /* Failed to execute the command */
                  return ("");
               }
            }
               
        case TOK_WHICH : return (findExecs(token->text()));
        
        default        : return (token->text());
    }
}





/************* END OF FILE **************************************************/
