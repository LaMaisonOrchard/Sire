//////////////////////////////////////////////////////////////////
//
//  Copyright 2024 david@the-hut.net
//  All rights reserved
//
@safe:

import std.stdio;
import std.ascii;
import std.exception;
import std.format;
import std.file;
import std.path;
import std.typecons;

import std.conv : to;

public
{
    string[] genIncludes(string file, string[] args)
    {
        auto env = parseArgs(dirName(file), args);
        auto inc = parseIncludes(file, env.defines);
        string[] deps;
        
        foreach (incFile ; inc)
        {
            bool found = false;
            foreach (path ; env.path)
            {
                string combined = chainPath(path, incFile).to!string();
                string fullPath = asAbsolutePath(absolutePath(combined)).to!string();
                if (exists(fullPath))
                {
                    found = true;
                    deps ~= fullPath;
                    break;
                }
            }
            
            if (!found)
            {
                deps ~= incFile;
            }
        }

        return deps;
    }
}


private
{
    string[] parseIncludes(string file, string[string] defines) @trusted
    {
        string[] rtn;
        
        try
        {
            foreach (string line; lines(File(file, "r")))
            {
                size_t i = 0;
                
                // Strip space
                while ((i < line.length) && isWhite(line[i])) {i += 1;}
                
                // Check for pre-processor line
                if ((i < line.length) && (line[i] == '#'))
                {
                    i += 1;

                    // Strip space
                    while ((i < line.length) && isWhite(line[i])) {i += 1;}
                    
                    size_t start = i; 
                    
                    // Look for name
                    while ((i < line.length) && isAlpha(line[i])) {i += 1;}
                                       
                    string name = line[start .. i];
                    
                    // Strip space
                    while ((i < line.length) && isWhite(line[i])) {i += 1;}
                    
                    string args = line[i .. $];
                    
                    switch (name)
                    {
                        case "if":
                            // TODO - consider support
                            break;
                            
                        case "ifdef":
                            // TODO - consider support
                            break;
                            
                        case "ifndef":
                            // TODO - consider support
                            break;
                            
                        case "else":
                            // TODO - consider support
                            break;
                            
                        case "elif":
                            // TODO - consider support
                            break;
                            
                        case "endif":
                            // TODO - consider support
                            break;
                            
                        case "define":
                            // TODO - consider support
                            break;
                            
                        case "include":
                            if (args.length == 0)
                            {
                                // No args - ignore
                            }
                            else if (args[0] == '<')
                            {
                                // System include - ignore for now
                            }
                            else if (args[0] == '\"')
                            {
                                // Local include - processor
                                size_t j = 1;
                                while ((j < args.length) && (args[j] != '\"')) {j += 1;}
                                if ((j < args.length) && (j > 2) && (args[j] == '\"'))
                                {
                                    // Include file
                                    rtn ~= args[1..j];
                                }
                            }
                            else
                            {
                                // invalid include - ignore
                            }
                            break;
                            
                        default:
                            // Unsupported so ignore it
                            break;
                    }
                }
            }
        }
        catch (ErrnoException ex)
        {
        }
  
        return rtn;
    }

    Tuple!(string[], "path", string[string], "defines") parseArgs(string path, string[] args)
    {
        string[]       includePath = [path];
        string[string] defines;
    
        foreach (arg ; args)
        {
            if (arg.length > 2)
            {
                if (arg[0..2] == "-I")
                {
                    // Include directory
                    string incPath = arg[2..$];
                    
                    if (isAbsolute(incPath) || (incPath[0] == '/') || (incPath[0] == '\\'))
                    {
                        includePath ~= incPath;
                    }
                    else
                    {
                        includePath ~= chainPath(path, incPath).to!string();
                    }
                }
                
                if (arg[0..2] == "-D")
                {
                    // Include directory
                    string define = arg[2..$];
                    
                    int i = 0;
                    for (i = 1; (i < define.length) && (define[i] != '='); i +=1)
                    {
                    }
                    
                    string name  = define[0 .. i];
                    string value = "";
                    
                    if ((i+1) < define.length)
                    {
                        value = define[i+1 .. $];
                    }
                    
                    defines[name] = value;
                }
            }
        }
        
        return Tuple!(string[], "path", string[string], "defines")(includePath, defines);
    }
}


