//////////////////////////////////////////////////////////////////
//
//  Copyright 2024 david@the-hut.net
//  All rights reserved
//
@safe:

import std.stdio;
import std.ascii;
import std.format;
import std.file;
import std.path;
import std.uni;
import std.typecons;

import std.conv : to;

public
{
    string[] genIncludes(string file, string[] args)
    {
        auto env = parseArgs(dirName(file), args);
        string[] deps;

        //@@ TODO Find include dependents
        writeln("@@ TODO INCLUDE ", file, " :: ", env.path, " :: ", env.defines);

        return deps;
    }
}


private
{
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


