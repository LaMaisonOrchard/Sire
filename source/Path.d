//////////////////////////////////////////////////////////////////
//
//  Copyright 2024 david@the-hut.net
//  All rights reserved
//

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

string FindExe(string[] path, string name)
{
    foreach (dir ; path)
    {
        version(Windows)
        {
            string rtn = dir ~ "/" ~ name;
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
            string rtn = dir ~ "/" ~ name;
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
    if ((match.length >= 5) && (match[0..5] == "deps:"))
    {
        // Check the dependentcies
        return FindDeps(SplitLine(match[5..$]));
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
					foreach (entry ; dirEntries(root, step, SpanMode.shallow, true))
					{
						next ~= entry.name();
					}
				}
			}
			
			rtn = next;
		}
	}
		
	return rtn;
}

string TmpFile()
{
    // random id with 10 letters
    auto id = letters.byCodeUnit.randomSample(10).to!string;
    return tempDir.buildPath(id ~ "_script");
}

private
{
    string[] FindDeps(string[] cmd)
    {
        string[] rtn;

        // TODO
        
        return rtn;
    }
    
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
        }

        return rtn;
    }
}
