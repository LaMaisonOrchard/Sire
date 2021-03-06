/*
 *      msh.cpp
 *      
 *      Copyright 2008 David Orchard <david@errol.org.uk>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include "sys.h"
#include "mlex.h"
#include "msh.h"


int msh(char const *script, char const **argv, char **envp)
{
    text t(script);
    Tmsh msh_parser(t);
	
    return msh_parser.state();
}



int Tmsh::parser()
{
    DlastRtn = 0;
    
    parseState_t rtn = parseStatements(TRUE, FALSE);
    
    if ((FILE_END != rtn) && (FILE_EXIT != rtn))
    {
        printE("<END> : End of IF Statement without IF Statement\n");
    }
    
    return DlastRtn;
}


Tlex *Tmsh::newLex(char const *file) {return new Tmlex(file);}
Tlex *Tmsh::newLex(FILE       *file) {return new Tmlex(file);}

parseState_t Tmsh::parseStatements(int active, int restricted)
{
    Ttoken *token;

    token = Dinput->getToken();
    while ((NULL != token) && TOK_COMMENT == token->type())
    {
      delete token;
      token = Dinput->getToken();
    }

    while (!doExit() && (TOK_END != token->type()))
    {
        Dinput->putBackToken(token);

        if (parseIf(active, restricted))
        {
            // If
        }
        else if (parseElse(active))
        {
            // Else
            return(PARSE_ELSE);
        }
        else if (parseElseIf(active))
        {
            // ElseIf
            return(PARSE_ELSE_IF);
        }
        else if (parseEndIf(active))
        {
            // EndIf
            return(PARSE_END_IF);
        }
        else
        {
            token = Dinput->getToken();

            switch (token->type())
            {
                case TOK_ECHO:
                {
                    delete token;
                    token = Dinput->getToken();
                    
                    while ((NULL    != token) &&
                           (TOK_EOL != token->type()) &&
                           (TOK_END != token->type()))
                    {
                        if (active) printT("%s ", token->text());
                        delete token;
                        token = Dinput->getToken();
                    }
                    if (active) printT("\n");
                }
                break;
                
                case TOK_CHANGE_DIR:
                {
                    delete token;
                    token = Dinput->getToken();
                    
                    if ((TOK_EOL != token->type()) &&
                        (TOK_END != token->type()))
                    {
                        // Change to the specified directory
                        if (active)
                        {
                            if (setDir(token->text()))
                            {
                                printT("Invalid directory\n");
                            }
                        }

                        // Strip to the end of line
                        strip(token);
                    }
                    else
                    {
                        // Change to the default directory
                    }
                }
                break;
            
                case TOK_PRINT_DIR:
                {
                    // Print the current working directory
                    if (active)
                    {
                        printT("%s\n", getDir());
                    }

                    // Strip to the end of line
                    strip(token);
                }
                break;
                
                case TOK_EXIT:
                {
                    delete token;
                    setExit();
                    return (FILE_EXIT);
                }
            
                case TOK_END:
                case TOK_EOL:
                {
                }
                break;
            
                default:
                {
                    char const *prog;
                    char const *list[1024];
                    int  idx = 0;
                    
                    // Generic command
                    while ((NULL    != token) &&
                           (TOK_EOL != token->type()) &&
                           (TOK_END != token->type()))
                    {
                         list[idx++] = strdup(token->text());
                         
                         delete token;
                         token = Dinput->getToken();
                    }
                    
                    if (active && (idx > 0))
                    {
                        prog = findExec (list[0]);
                        list[idx++] = NULL;
                        
                        // Execute the command
                        DlastRtn = MYspawnve(true, (char*)prog, (char const **)list, NULL);
                        
                        // Free the space
                        for (idx -= 1; (0 <= idx); idx -= 1)
                        {
                            free((void*)list[idx]);
                        }
                    }
                }
                break;
            }
        }

        token = Dinput->getToken();
        while ((NULL != token) && TOK_COMMENT == token->type())
        {
            delete token;
            token = Dinput->getToken();
        }
    }

    delete token;

    return (FILE_END);
}



void Tmsh::strip(Ttoken *token)
{
    // Strip to the end of line
    
    while ((NULL    != token) &&
           (TOK_EOL != token->type()) &&
           (TOK_END != token->type()))
    {
        delete token;
        token = Dinput->getToken();
    }
}

