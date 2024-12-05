//////////////////////////////////////////////////////////////////
//
//  Copyright 2024 david@the-hut.net
//  All rights reserved
//

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

class Enviro
{
    this(bool blind = false, string[] param = [])
    {
        this.base  = null;
        this.param = param;
        this.blind = blind;
    }
    
    this(Enviro base, bool blind, string[] param = [])
    {
        this.base  = base;
        this.param = param;
        this.blind = blind;
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
                    auto tmp = SplitText(environment.get(split.name, null), pathSeparator[0]);
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
            
            if (!this.blind)
            {
                auto tmp = BuildText(value, pathSeparator[0]);
                if (tmp is null)
                {
                    tmp = "";
                }
                environment[name] = tmp;
            }
        }
    }

    private
    {
        Enviro base;
        string[] param;
        bool blind;        // Don't update the system environment
	
	    string[][string] localVar;
    }
}


const(char)[] expandScript(string text, string[] param)
{
	static auto buffer = appender!(char[]);
	static auto word = appender!(char[]);
	buffer.clear();
	
	bool first = true;
	for (int i = 0 ; (i < text.length); i += 1)
	{
		if (isWhite(text[i]))
		{
			buffer.put(text[i]);
		}
		else if (text[i] == '\"')
		{
			// quoted name
			word.clear();
			//word.add(text[i]);
			for (; (i < text.length); i += 1)
			{
				if (text[i] == '\"')
				{
					//word.add(text[i]);
					//buffer ~= PackageList(ExpandVar(word.data().idup, param));
				}
				else if ((text[i] == '\r') || (text[i] == '\n'))
				{
					//word.add('\"');
					//buffer ~= PackageList(ExpandVar(word.data().idup, param));
				}
				else
				{
					//word.add(text[i]);
				}
			}
		}
		else
		{
			// name
			word.clear();
			//word.add(text[i]);
			for (; (i < text.length); i += 1)
			{
				if (isWhite(text[i]))
				{
					//buffer ~= PackageList(ExpandVar(word.data().idup, param));
					//buffer.add(text[i]);
				}
				else
				{
					//word.add(text[i]);
				}
			}
		}
	}
		
	return buffer.data();
}


string[] ExpandVar(const(char)[] txt, Enviro env)
{
    string[] rtn;

    if (txt.length > 2)
    {
        for (int i = 0; (i < txt.length-2); i += 1)
        {
            if (txt[i..i+2] == "${")
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

                return rtn;
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
            if (txt[i] == '{')
            {
                count += 1;
            }
            if (txt[i] == '}')
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
                        if (name[end] == '-')
                        {
                            negative = true;
                            end += 1;
                        }
                        
                        start = end;
                        while ((end < name.length) && isDigit(name[end]))
                        {
                            end += 1;
                        }
                        
                        if (end < name.length)
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
        
        if (list is null)
        {
            rtn ~= "";
        }
        else
        {
            if (end > 0)
            {
                if ((start < end) &&
                    (end <= list.length))
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
