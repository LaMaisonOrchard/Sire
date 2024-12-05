import std.stdio;
import std.file;
import std.datetime;

import Sirefile;
import EnvVar;

int main(string[] args)
{
    string sirefile = "Sirefile.txt"; // Default
    string[] targets;
    
    for (int i = 1; (i < args.length); i += 1)
    {
        switch (args[i])
        {
            case "--help":
                writeln("--in <sirefile> -C source");
                return 0;
                
            case "--in":
                i += 1;
                if (i < args.length)
                {
                    sirefile = args[i];
                }
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

    if (targets.length == 0)
    {
        targets = ["TARGET"];
    }

    Sirefile.Sirefile config;

    try
    {
        if (sirefile == "-")
        {
            config = new Sirefile.Sirefile(stdin);
        }
        else
        {
            config = new Sirefile.Sirefile(sirefile);
        }
    }
    catch (Exception ex)
    {
        writeln("Illegal sirefile [", ex.msg , "]");
        return -3;
    }

    config.Pre();

    SysTime time;
    foreach (target ; targets)
    {
        time = config.Build(target);
        if (time == SysTime())
        {
            break;
        }
    }
    config.Post();

    return (time == SysTime())?(-1):(0);
}
