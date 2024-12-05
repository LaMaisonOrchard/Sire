import std.stdio;
import std.ascii;
import std.array;
import std.typecons;


bool ParseInt(const(char)[] text, ref uint index)
{
	index = 0;
	foreach (char ch ; text)
	{
		if (isDigit(ch))
		{
			index *= 10;
			index += (ch - '0');
		}
		else
		{
			// not an integer
			return false;
		}
	}
	return true;
}

string[] SplitWildcards(string text)
{
    string[] rtn;
    string   build;
    int start = 0;
    for (int i = 0; (i < text.length); i += 1)
    {
        if (text[i] == '*')
        {
            if (build.length > 0)
            {
                rtn ~= build;
                build = "";
            }
            if (i > 0)
            {
                if (text[i-1] != '*')
                {
                    rtn ~= "*";
                    start = i+1;
                }
            }
            else
            {
                rtn ~= "*";
                start = i+1;
            }
        }
        else if (text[i] == '\\')
        {
            i += 1;
            if (i < text.length)
            {
                build ~= text[i];
            }
        }
        else
        {
            build ~= text[i];
        }
    }
    if (build.length > 0)
    {
        rtn ~= build;
    }

    foreach(entry ; rtn)
    {
        write(entry, "::");
    }
    writeln();

    return rtn;
}

string[] SplitText(string text, const(char) sep)
{
    string[] rtn;

    int start = 0;
    int end = 0;
    while (end < text.length)
    {
        if (text[end] == sep)
        {
            if (end > start)
            {
                rtn ~= text[start .. end];
                start = end+1;
            }
        }
        end += 1;
    }

    if (end > start)
    {
        rtn ~= text[start .. end];
    }

    return rtn;
}

string BuildText(string[] text, const(char) sep)
{
    string rtn = "";

    for (int i = 0; (i < text.length); i += 1)
    {
        if (i > 0)
        {
            rtn ~= sep;
        }
        
        rtn ~= text[i];
    }

    return rtn;
}


    
string[] SplitLine(string cmd)
{
    string[] rtn;
        
    int idx   = 0;

    bool inString = false;
        
    auto built = appender!string();

    while (idx < cmd.length)
    {
        if (inString && (cmd[idx] == '\\'))
        {
            idx += 1;
            if (idx < cmd.length)
            {
                switch (cmd[idx])
                {
                case 'n':
                    built.put('\n');
                    break;
                        
                case 'r':
                    built.put('\r');
                    break;
                        
                case 't':
                    built.put('\t');
                    break;
                        
                default:
                    built.put(cmd[idx]);
                    break;
                }
                idx += 1;
            }
        }
        else if (cmd[idx] == '\"')
        {
            // Toggle the string state
            if (inString)
            {
                rtn ~= built[];
                built = appender!string();
            }
            else
            {
                if (built[].length > 0)
                {
                    rtn ~= built[];
                    built = appender!string();
                }
            }
            inString = !inString;
            idx += 1;
        }
        else if (inString)
        {
            built.put(cmd[idx]);
            idx += 1;
        }
        else if (!isWhite(cmd[idx]))
        {
            built.put(cmd[idx]);
            idx += 1;
        }
        else
        {
            if (built[].length > 0)
            {
                rtn ~= built[];
                built = appender!string();
            }
                
            // Ignore white space
            idx += 1;
        }
    }
        
    if (built[].length > 0)
    {
        rtn ~= built[];
        built = appender!string();
    }

    return rtn;
}

Tuple!(bool, "matched", string[], "parts") Match(string match, string text)
{
    bool matched = false;
    string[] parts;

    int start = 0;
    int end   = 0;
    int i     = 0;

    while ((i < match.length) && (i < text.length))
    {
        if (match[i] == '*')
        {
            start = i;
            end   = i;
            i += 1;

            while ((i < match.length) && (match[i] == '*'))
            {
                 parts ~= "";
                 i += 1;
            }

            if (i >= match.length)
            {
                parts ~= text[start .. $];
                matched = true;
                return Tuple!(bool, "matched", string[], "parts")(matched, parts);
            }
            
            while (end < text.length)
            {
                while ((end < text.length) && (match[i] != text[end]))
                {
                    end += 1;
                }

                if (end >= text.length)
                {
                    return Tuple!(bool, "matched", string[], "parts")(matched, parts);
                }

                auto result = Match(match[i .. $], text[end .. $]);

                if (result.matched)
                {
                    parts ~= text[start .. end];
                    parts ~= result.parts;
                    matched = true;
                    return Tuple!(bool, "matched", string[], "parts")(matched, parts);
                }

                end += 1;
            }
        }
        else if ((i >= match.length) || (match[i] != text[i]))
        {
            // No match
            return Tuple!(bool, "matched", string[], "parts")(matched, parts);
        }
        else
        {
            i += 1;
        }
    }

    matched = (match.length == text.length);
                
    return Tuple!(bool, "matched", string[], "parts")(matched, parts);
}

unittest
{
    auto result = Match("fred", "fred");
    assert(result.matched);
    
    string[] list1;
    string[] list2 = result.parts;
    assert(list1 == list2);
}

unittest
{
    auto result = Match("fred1", "fred");
    assert(!result.matched);
    
    string[] list1;
    string[] list2 = result.parts;
    assert(list1 == list2);
}

unittest
{
    auto result = Match("fred", "fred1");
    assert(!result.matched);
    
    string[] list1;
    string[] list2 = result.parts;
    assert(list1 == list2);
}

unittest
{
    auto result = Match("", "fred");
    assert(!result.matched);
    
    string[] list1;
    string[] list2 = result.parts;
    assert(list1 == list2);
}

unittest
{
    auto result = Match("fred", "");
    assert(!result.matched);
    
    string[] list1;
    string[] list2 = result.parts;
    assert(list1 == list2);
}

unittest
{
    auto result = Match("*", "fred");
    assert(result.matched);
    
    string[] list1 = ["fred"];
    string[] list2 = result.parts;
    assert(list1 == list2);
}

unittest
{
    auto result = Match("fred", "*");
    assert(!result.matched);
    
    string[] list1;
    string[] list2 = result.parts;
    assert(list1 == list2);
}

unittest
{
    auto result = Match("**", "fred");
    assert(result.matched);
    
    string[] list1 = ["", "fred"];
    string[] list2 = result.parts;
    assert(list1 == list2);
}

unittest
{
    auto result = Match("*fred", "fred");
    assert(result.matched);
    
    string[] list1 = [""];
    string[] list2 = result.parts;
    assert(list1 == list2);
}

unittest
{
    auto result = Match("*red", "fred");
    assert(result.matched);
    
    string[] list1 = ["f"];
    string[] list2 = result.parts;
    assert(list1 == list2);
}

unittest
{
    auto result = Match("f*", "fred");
    assert(result.matched);
    
    string[] list1 = ["red"];
    string[] list2 = result.parts;
    assert(list1 == list2);
}

unittest
{
    auto result = Match("f*ck", "fred");
    assert(!result.matched);
    
    string[] list1 = [];
    string[] list2 = result.parts;
    assert(list1 == list2);
}

unittest
{
    auto result = Match("f*e*d", "fred");
    assert(result.matched);
    
    string[] list1 = ["r", ""];
    string[] list2 = result.parts;
    assert(list1 == list2);
}

private
{
}
