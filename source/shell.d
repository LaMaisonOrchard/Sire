//////////////////////////////////////////////////////////////////
//
//  Copyright 2024 david@the-hut.net
//  All rights reserved
//
@safe:


import std.stdio;
import std.array;
import std.path;
import std.file;
import std.ascii;
import std.process;
import std.container;
import EnvVar;
import Path;
import TextUtils;


class ShellException : Exception
{
    this(string msg)
    {
        super(msg);
    }
}

string[] RunLine(string[] cmd)
{
    if (cmd.length == 0)
    {
        throw new ShellException("Illegal command");
    }
    else if (exists(cmd[0]))
    {
        auto state = execute (cmd);
        
        if (state.status == 0)
        {
            return SplitLine(state.output);
        }
        else
        {
            throw new ShellException("Command [" ~ BuildText(cmd, ' ') ~ "] failed");
        }
    }
    else
    {
        throw new ShellException(cmd[0] ~ " Does not exist");
    }
}

bool Execute(string[] cmd)
{
    if (cmd.length == 0)
    {
        throw new ShellException("Illegal command");
    }
    else if (exists(cmd[0]))
    {
        auto state = execute (cmd);
        writeln(state.output);
        return (state.status == 0);
    }
    else
    {
        throw new ShellException(cmd[0] ~ " Does not exist");
    }
}


bool Execute(string script, ref Enviro env, bool quiet) 
{
    bool rtn = false;
    
    auto expanded = appender!string();

    // Expand the script
    int start = 0;
    int end   = 0;
    while (end < script.length)
    {
        // Copy the white space
        while ((end < script.length) && isWhite(script[end]))
        {
            expanded.put(script[end]);
            end += 1;
        }
        start = end;

        // Find the token
        while ((end < script.length) && !isWhite(script[end]))
        {
            end += 1;
        }

        if (end > start)
        {
            // If we have a token then expand it
            auto list = ExpandVar(script[start .. end], env);
            for (int i = 0; (i < list.length); i += 1)
            {
                if (i > 0)
                {
                    expanded.put(" ");
                }
                expanded.put(list[i]);
            }
        }
    }

    if (!quiet)
    {
        writeln(expanded[]);
    }

    try
    {
        if (env.Get("SHELL") == ["sire"])
        {
            rtn = InternalShell(expanded[], env);
        }
        else
        {
            string[] cmd = env.Get("SHELL");

            // Create the script file
            string file = TmpFile();
            std.file.write(file, expanded[]);
            scope(exit) std.file.remove(file);

            cmd ~= file;

            rtn = Execute(cmd);
        }
    }
    catch (Exception ex)
    {
        writeln(ex.msg);
        rtn = false;
    }

    if (!rtn)
    {
        writeln("ERROR");
    }

    return rtn;
}

private
{
    bool InternalShell(string script, ref Enviro env)
    {
        bool rtn = true;
        string[] lines;

        SList!string cwdStack;

        // Read the lines
        int idx = 0;
        string line;
        while ((line = ReadLine(script, idx)) !is null)
        {
            lines ~= line;
        }

        // Process the script
        idx = 0;
        while (idx < lines.length)
        {
            string[] args = SplitLine(ExpandLine(lines[idx], env));

            if ((args.length == 3) && (args[1] == "="))
            {
                env.Set(args[0], SplitText(args[2], pathSeparator[0]));
            }
            else if (args.length > 0)
            {
                switch (args[0])
                {
                case "echo":
                    writeln(BuildText(args[1..$], ' '));
                    break;
                
                case "mkdir":
                    foreach (dirEntries ; args[1..$])
                    {
                        mkdirRecurse(dirEntries);
                    }
                    break;
                
                case "cd":
                    if (args.length == 2)
                    {
                        cwdStack.insertFront(env.Get("PWD")[0]);
                        env.Set("PWD", [args[1]]);
                        
                        writeln("Now in : ", env.Get("PWD")[0]);
                    }
                    break;
                
                case "cp":
                case "copy":
                    if (args.length > 2)
                    {
                        Path.CopyFiles(args[1..$]);
                    }
                    break;
                
                case "pop":
                    if (!cwdStack.empty())
                    {
                        env.Set("PWD", [cwdStack.front()]);
                        cwdStack.removeFront();
                        
                        writeln("Now in : ", env.Get("PWD")[0]);
                    }
                    else
                    {
                        // Error
                        rtn = false;
                    }
                    break;
                
                default:
                    args[0] = FindExe(env.Get("PATH"), args[0]);
                    rtn = Execute(args);
                    break;
                }
            }
            
            idx += 1;
        }

        // Rewind back to the start
        while (!cwdStack.empty())
        {
            env.Set("PWD", [cwdStack.front()]);
            cwdStack.removeFront();
            
            writeln("Now in : ", env.Get("PWD")[0]);
        }

        return rtn;
    }

    string ReadLine(string script, ref int idx)
    {
        int start = idx;
        int end   = idx;

        string rtn = null;

        while ((idx < script.length) && (start == end))
        {
            // Strip leading spaces
            while ((idx < script.length) && isWhite(script[idx]))
            {
                idx += 1;
            }

            start = idx;
            
            // Read to end of command
            while ((idx < script.length) &&
                   (script[idx] != '#') &&
                   (script[idx] != '\n') &&
                   (script[idx] != '\r'))
            {
                idx += 1;
            }

            end = idx;
            
            // Read to end of line
            while ((idx < script.length) &&
                   (script[idx] != '\n') &&
                   (script[idx] != '\r'))
            {
                idx += 1;
            }
        }

        if (start != end)
        {
            rtn = script[start .. end];
        }

        return rtn;
    }

    string ExpandLine(string line, Enviro env)
    {
        auto expanded = appender!string();

        int start = 0;
        int idx   = 0;

        while (idx < line.length)
        {
            // Copy to evironment variable
            while ((idx < line.length) && (line[idx] != '$'))
            {
                idx += 1;
            }

            if (idx+4 < line.length)
            {
                if ((line[idx+0] == '$') &&
                    (line[idx+1] == '('))
                {
                    // Insert the bit before the variable
                    expanded.put(line[start .. idx]);
                    idx += 2;
                    start = idx;
                }
                
                while ((idx < line.length) && (line[idx] != ')'))
                {
                    idx += 1;
                }

                if (idx < line.length)
                {
                    expanded.put(BuildText(env.Get(line[start .. idx]), pathSeparator[0]));
                    idx += 1;
                    start = idx;
                }
                else
                {
                    // Error - Badly formed variable
                }
            }
            
            expanded.put(line[start .. idx]);
        }

        return expanded[];
    }
}

