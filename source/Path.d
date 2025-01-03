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

import std.conv : to;
import std.random : randomSample;
import std.utf : byCodeUnit;

import EnvVar;
import TextUtils;
import shell;


class PathException : Exception
{
    this(string msg)
    {
        super(msg);
    }
}

string FindExe(string[] path, string name)
{
    if (name.length == 0)
    {
        throw new PathException("Missing executable name");
    }

    if (isAbsolute(name))
    {
        // Nothing to look for
        
        if (exists(name))
        {
            return name;
        }
        else
        {
            return null;
        }
    }
    
    foreach (dir ; path)
    {
        version(Windows)
        {
            string rtn = chainPath(dir, name).to!string();
            rtn = absolutePath(rtn);
            
            if (icmp(extension(name), ".exe") == 0)
            {
                if (exists(rtn))
                {
                    return rtn;
                }
            }
            else  if (icmp(extension(name), ".bat") == 0)
            {
                if (exists(rtn))
                {
                    return rtn;
                }
            }
            else 
            {
                if (exists(rtn ~ ".exe"))
                {
                    return rtn ~ ".exe";
                }
                if (exists(rtn ~ ".bat"))
                {
                    return rtn ~ ".bat";
                }
            }
        }
        else
        {
            string rtn = chainPath(dir, name).to!string() ;
            rtn = absolutePath(rtn);
            if (exists(rtn))
            {
                return rtn;
            }
        }
    }
    return null;
}

string[] FindFiles(string match)
{
    if ((match.length >= 5) && (match[0..5] == "run:"))
    {
        // Check the dependentcies
        return RunLine(SplitLine(match[5..$]));
    }
    else
    {
        return BuildFiles(match);
    }
}

string [] ExpandFile(const(char)[] text)
{
	string[] rtn = [rootName(text.idup)];
	
	if (rtn[0] is null)
	{
		rtn[0] = "";
	}
	else
	{
		text = text[rtn[0].length .. $];
	}
	
	auto paths = pathSplitter(text.idup);
	
	foreach (step; paths)
	{
		if (step == ".")
		{
		}
		else if (step == "..")
		{
			foreach (ref string root; rtn)
			{
				if (root.length != 0)
				{
					root ~= "/";
				}
				root ~= "..";
			}
		}
		else
		{
			string[] next = [];
			foreach (root; rtn)
			{
				if ((root.length == 0) || isDir(root))
				{
                    () @trusted
                    {
    					foreach (entry ; dirEntries(root, step, SpanMode.shallow, true))
    					{
    						next ~= entry.name();
    					}
    				}();
                }
			}
			
			rtn = next;
		}
	}
		
	return rtn;
}

void CopyFiles(scope string[] files)   // MAKE SURE THIS IS SAFE
{
    assert(files.length >= 2);
    
    string   dest    = files[files.length-1];
    string[] sources = files[0..$-1];

    if ((sources.length == 1) &&
        exists(sources[0]) &&
        !isDir(sources[0]) &&
        (!exists(dest) || !isDir(dest)))
    {
        // file to file copy
        copy(sources[0], dest);
        //writeln("Copy ", sources[0], " ==> ", dest);
    }
    else
    {
        if (!exists(dest))
        {
            mkdirRecurse(dest);
            //writeln("Mkdir ", dest);
        }

        foreach (source ; sources)
        {
            scope string base = absolutePath(source);
            
            if (!exists(source))
            {
                writeln("Error : No such file : ", source);
            }
            else if (isDir(source))
            {
                () @trusted
                {
                    foreach (string file ; dirEntries(source, SpanMode.breadth))
                    {
                        scope string root = asRelativePath(absolutePath(file), base).to!string();

                        if (isDir(file))
                        {
                            mkdirRecurse(chainPath(dest, root).to!string());
                            //writeln("Mkdir ", chainPath(dest, root));
                        }
                        else
                        {
                            copy(file, chainPath(dest, root));
                            //writeln("Copy ", file, " ==> ", chainPath(dest, root));
                        }
                    }
                }();
            }
            else
            {
                copy(source, dest ~ "/" ~ source);
            }
        }
    }
}

string TmpFile()
{
    // random id with 10 letters
    auto id = letters.byCodeUnit.randomSample(10).to!string;
    return tempDir.buildPath(id ~ "_script");
}

private
{
    string[] BuildFiles(string match)
    {
        string[] rtn;
        
        // Find the leading sections without wildcards
        int end = 0;
        for (int i = 0; (i < match.length); i += 1)
        {
            if (match[0] == '\\')
            {
                end = i;
            }
            else if (match[0] == '/')
            {
                end = i;
            }
            else if (match[0] == '*')
            {
                break;
            }
        }
        
        if (end == match.length)
        {
            // No wild cards (litteral name)
            rtn ~= match;
        }
        else  if (end > 0)
        {
            // Leading sections without wildcards
            rtn = BuildFiles(match[0 .. end], match[end+1..$]);
        }
        else if (match[0] == '\\')
        {
            // Absolute path
            rtn = BuildFiles("/", match[1..$]);
        }
        else if (match[0] == '/')
        {
            // Absolute path
            rtn = BuildFiles("/", match[1..$]);
        }
        else
        {
            // Relative path
            foreach (file ; BuildFiles(".", match))
            {
                rtn ~= file[2 ..$];
            }
        }

        return rtn;
    }
    
    string[] BuildFiles(string base, string match)
    {
        string[] rtn;
    
        int end = 0;
        for (end = 0; (end < match.length); end += 1)
        {
            if (match[end] == '\\')
            {
                break;
            }
            if (match[end] == '/')
            {
                break;
            }
        }

        if ((match[0 .. end] == ".") || (match[0 .. end] == ".."))
        {
            rtn ~= BuildFiles(base ~ '/' ~ match[0 .. end], match[end+1 .. $]);
        }
        else
        {
            () @trusted
            {
                foreach (entry ; dirEntries(base, match[0 .. end], SpanMode.shallow))
                {
                    if (end < match.length)
                    {
                        rtn ~= BuildFiles(base ~ '/' ~ baseName(entry.name()), match[end+1 .. $]);
                    }
                    else
                    {
                        rtn ~= (base ~ '/' ~ baseName(entry.name()));
                    }
                }
            }();
        }

        return rtn;
    }
}
