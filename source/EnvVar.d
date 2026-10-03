//////////////////////////////////////////////////////////////////
//
//  Copyright 2024 david@the-hut.net
//  All rights reserved
//
@safe:

import std.stdio;
import std.ascii;
import std.array;
import std.format;
import std.process;
import std.file;
import std.path;
import std.typecons;

import TextUtils;

class EnviroException : Exception
{
    this(string msg)
    {
        super(msg);
    }
}

string GetEnv(const(char)[] name)
{
    return environment.get(name, null);
}

class Enviro
{
    this(string[] param = [])
    {
        this.base  = null;
        this.param = param;
    }
    
    this(Enviro base, string[] param = [])
    {
        this.base  = base;
        this.param = param;
    }

    void EnableRestore()
    {
        this.isRewindable = true;
    }

    void Restore()
    {
        this.isRewindable = false;

        foreach (name, value ; this.rewind)
        {
            this.Set(name, value);
        }

        // Reset the arraY
        this.rewind = (string[][string]).init;
    }

    string[] Get(const(char)[] name)
    {
        string[] rtn;

        uint index;
	
    	if (ParseInt(name, index))
    	{ 
            if (index < this.param.length)
        	{
        		// Insert the parameter
        		rtn ~= this.param[index];
        	}
    	}
        else
        {
            try
            {
                auto split = ParseName(name);
            
                if (split.name == "PWD")
                {
                    rtn ~= getcwd();
                }
                else if (split.name in this.localVar)
                {
                    rtn ~= SpliceList(this.localVar[split.name], split.start, split.end);
                }
                else if (this.base !is null)
                {
                    rtn = this.base.Get(name);
                }
                else
                {
                    auto tmp = SplitText(GetEnv(split.name), pathSeparator[0]);
                    rtn ~= SpliceList(tmp, split.start, split.end);
                }
            }
            catch (EnviroException ex)
            {
                writeln("Error ", name, " --- ", ex.msg);
            }
        }
            
        return rtn;
    }
    
    void   Set(const(char)[] name, string[] value)
    {
        if (this.isRewindable && ((name in this.rewind) is null))
        {
            // Record the original value
            this.rewind[name] = Get(name);
        }
        
        if (name == "PWD")
        {
            try
            {
                chdir(value[0]);
            }
            catch(Exception ex)
            {
            }
        } 
        else
        {
            localVar[name] = value;
            
            auto tmp = BuildText(value, pathSeparator[0]);
            if (tmp is null)
            {
                tmp = "";
            }
            environment[name] = tmp;
        }
    }

    private
    {
        Enviro base;
        string[] param;
        bool isRewindable;        // Don't update the system environment
	
	    string[][string] localVar;
	    string[][string] rewind;
    }
}

string[] ExpandVar(const(char)[] txt, Enviro env)
{
    return ExpandVar(txt, env, 0, 0);
}

string[] ExpandVar(const(char)[] txt, Enviro env, ulong from, ulong to)
{
    string[] rtn;

    if (txt.length > 2)
    {
        for (int i = 0; (i < txt.length-2); i += 1)
        {
            if (txt[i..i+2] == "$(")
            {
                const(char)[] pre = txt[0..i];
                int j = ScanToMatchingBrace(txt[i+2..$]);
                string[] mid = ExpandVar(txt[i+2 .. j+i+1], env);
                string[] post = ExpandVar(txt[j+i+2 .. $], env);

                foreach(m ; mid)
                {
                    foreach (v ; env.Get(m))
                    {
                        foreach (p ; post)
                        {
                            rtn ~= (pre ~ v ~ p).idup;
                        }
                    }
                }
                
                if (to <= from)
                {
                    to = rtn.length;
                }

                if (from >= rtn.length)
                {
                    return [];
                }
                else
                {
                    return rtn[from .. to];
                }
            }
        }
    }

    rtn ~= txt.idup;

    return rtn;
}

private
{
	alias ValueList = string[];
	
	ValueList[string] localVar;
	
	string[] GetVar(string text)
	{
		ValueList value = [];

		if (text in localVar)
		{
			auto tmp = localVar[text];
			if (tmp !is null)
			{
				value = tmp;
			}
		}
		else
		{
			auto tmp = environment.get(text, null);
			if (tmp !is null)
			{
				value = [tmp];
			}
		}
		
		return value;
	}

    int ScanToMatchingBrace(const(char)[] txt)
    {
        int count = 1;
        int i = 0;
        for (; (i < txt.length) && (count > 0); i += 1)
        {
            if (txt[i] == '(')
            {
                count += 1;
            }
            if (txt[i] == ')')
            {
                count -= 1;
            }
        }

        return i;
    }
    
    bool isNameChar(char ch)
    {
        return 
           (isAlphaNum(ch) || 
            (ch == '_')
            );
    }

    Tuple!(const(char)[], "name", int , "start", int, "end") ParseName(const(char)[] name)
    {
        // Parse
        // Name
        // Name:2
        // Name:2:3
        // Name:2:-1
        const(char)[] rtnName;
        int    rtnStart = 0;
        int    rtnEnd = 0;

        uint tmp;
        bool negative = false;

        int start = 0;
        int end   = 0;
        while ((end < name.length) && isNameChar(name[end]))
        {
            end += 1;
        }

        rtnName = name[start .. end];
 
        if (end < name.length)
        {
            if (name[end] != ':')
            {
                // Error
                throw new EnviroException("Bad index");
            }
            else
            {
                end += 1;
                start = end;
                while ((end < name.length) && isDigit(name[end]))
                {
                    end += 1;
                }

                ParseInt(name[start .. end], tmp);
                rtnStart = cast(int)(tmp);
                
                if (end < name.length)
                {
                    if (name[end] != ':')
                    {
                        // Error
                        throw new EnviroException("Bad index");
                    }
                    else
                    {
                        end += 1;
                        if ( (end < name.length) && (name[end] == '-'))
                        {
                            negative = true;
                            end += 1;
                        }
                        
                        start = end;
                        while ((end < name.length) && isDigit(name[end]))
                        {
                            end += 1;
                        }
                        
                        if ((start >= name.length ) || (end < name.length))
                        {
                            throw new EnviroException("Bad index");
                        }
                        
                        ParseInt(name[start .. end], tmp);
                        rtnEnd = (negative)?(-cast(int)(tmp)):(cast(int)(tmp));
                    }
                }
            }
        }
        
        return Tuple!(const(char)[], "name", int , "start", int, "end")(rtnName, rtnStart, rtnEnd);
    }

    string[] SpliceList(string[] list, int start, int end)
    {
        string[] rtn;
        
        if ((list is null) || (list.length == 0))
        {
            // Drop through
        }
        else
        {
            if (end > 0)
            {
                if (end > list.length)
                {
                    end = cast(int)(list.length);
                }
                
                if (start < end)
                {
                    rtn ~= list[start .. end];
                }
                else
                {
                    // Error
                }
            }
            else
            {
                if (start <= list.length + end)
                {
    		         rtn ~= list[start .. list.length + end];
                }
                else
                {
                    // Error
                }
            }
        }

        return rtn;
    }
}

unittest
{
    Enviro env = new Enviro();
    env.Set("FRED", ["A", "B", "C", "D"]);
    
    assert(env.Get("FRED") == ["A", "B", "C", "D"]);
    assert(env.Get("FRED:2") == ["C", "D"]);
    assert(env.Get("FRED:4") == []);
    assert(env.Get("FRED:-1") == []);
    
    assert(env.Get("FRED:1:2") == ["B"]);
    assert(env.Get("FRED:1:3") == ["B", "C"]);
    assert(env.Get("FRED:1:8") == ["B", "C", "D"]);
    assert(env.Get("FRED:1:-1") == ["B", "C"]);
    assert(env.Get("FRED:1:-3") == []);
    assert(env.Get("FRED:1:0") == ["B", "C", "D"]);
    assert(env.Get("FRED:2:1") == []);
}

unittest
{
    Enviro env = new Enviro();
    env.Set("FRED", []);
    
    assert(env.Get("FRED") == []);
    assert(env.Get("FRED:2") == []);
    assert(env.Get("FRED:4") == []);
    assert(env.Get("FRED:-1") == []);
    
    assert(env.Get("FRED:1:2") == []);
    assert(env.Get("FRED:1:3") == []);
    assert(env.Get("FRED:1:8") == []);
    assert(env.Get("FRED:1:-1") == []);
    assert(env.Get("FRED:1:-3") == []);
    assert(env.Get("FRED:1:0") == []);
    assert(env.Get("FRED:2:1") == []);
}
