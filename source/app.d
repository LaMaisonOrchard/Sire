//////////////////////////////////////////////////////////////////
//
//  Copyright 2024 david@the-hut.net
//  All rights reserved
//
//@safe:

import std.stdio;
import std.file;
import std.datetime;

import Sirefile;
import EnvVar;

string[] scriptFiles=
[
    "Sirefile",
    "sirefile",
    "Jakefile",
    "jakefile",
    "Sirefile.txt",
    "sirefile.txt"
];

int main(string[] args)
{
    string sirefile;
    string[] targets;

    foreach (name ; scriptFiles)
    {
        if (exists(name))
        {
            sirefile = name;
            break;
        }
    }

    OutputLevel quiet;
    switch (GetEnv("QUIET"))
    {
        case "0": quiet = OutputLevel.VERBOSE; break;
        case "1": quiet = OutputLevel.NORMAL; break;
        case "2": quiet = OutputLevel.QUIET; break;
        case "":  
        default:  quiet = OutputLevel.NORMAL; break;
    }
    
    for (int i = 1; (i < args.length); i += 1)
    {
        switch (args[i])
        {
            case "-h":
            case "-help":
            case "--help":
                writeln("--help");
                writeln("--version");
                writeln("[--in <sirefile>] [-C <path>] [--verbose | --normal | --quiet] {<targets>}");
                return 0;
                
            case "-i":
            case "--version":
                writeln("Sire 0.0.0");
                return 0;
                
            case "--in":
                i += 1;
                if (i < args.length)
                {
                    sirefile = args[i];
                }
                break;
                
            case "--verbose":
                quiet = OutputLevel.VERBOSE;
                break;
                
            case "--normal":
                quiet = OutputLevel.NORMAL;
                break;
                
            case "--quiet":
                quiet = OutputLevel.QUIET;
                break;
                
            case "-C":
                i += 1;
                if (i < args.length)
                {
                    chdir(args[i]);
                }
                break;
                
            default:
                targets ~= args[i];
                break;
        }
    }

    if (sirefile is null)
    {
        writeln("No Sirefile");
        return -1;
    }

    if (targets.length == 0)
    {
        targets = ["TARGET"];
    }

    Sirefile.Sirefile config;

    try
    {
        if (sirefile == "-")
        {
            config = new Sirefile.Sirefile(stdin, quiet);
        }
        else
        {
            config = new Sirefile.Sirefile(sirefile, quiet);
        }
    }
    catch (Exception ex)
    {
        writeln("Illegal sirefile [", ex.msg , "]");
        return -3;
    }

    config.Pre();

    SysTime time;
    try
    {
        foreach (target ; targets)
        {
            time = config.Build(target);
            if (time == SysTime())
            {
                writeln("Failed to build [", target , "]");
                break;
            }
        }

        if (time == SysTime())
        {
            config.Failed();
        }
        else
        {
            config.Post();
        }
    }
    catch (Exception ex)
    {
        writeln(ex.message());
        config.Failed();
        time = SysTime.init;
    }

    return (time == SysTime())?(-1):(0);
}
