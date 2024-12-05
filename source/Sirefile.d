import std.stdio;
import std.ascii;
import std.array;
import std.format;
import std.file;
import std.path;
import std.conv;
import std.typecons;
import std.datetime;

import EnvVar;
import Path;
import TextUtils;
import shell;

enum Flags : int
{
    CREATE = 1,
    TOUCH  = 2,
    FORCE  = 4,
    QUIET  = 8
};

class SireException : Exception
{
    this(string msg)
    {
        super(msg);
    }
}

class Sirefile
{
	this(File fp)
	{
        DefaultEnv();
        
        this.input = new InputStack();
        this.input.Push("<stdin>", fp);
        Parse();
	}
    
	this(string file)
	{
        DefaultEnv();
        
        this.input = new InputStack();
        auto fp = File(file, "r");
        this.input.Push(file, fp);
        Parse();
	}

    bool Pre()
    {
        bool rtn = true;

        if (this.preRule !is null)
        {
            rtn = this.preRule.Execute(this.env);
        }

        return rtn;
    }

    SysTime Build(string target)
    {
        if (target in resolved)
        {
            return resolved[target];
        }
        
        SysTime accessTime;
        SysTime targetTime;
        if (exists(target))
        {
            getTimes(target, accessTime, targetTime);
        }

        foreach (rule ; this.rules)
        {
            auto match = rule.Match(target);
            if (match.matched)
            {
                auto env = new Enviro(this.env, false, match.parts);

                auto deps = rule.Dependents(env);

                // Check dependents
                bool missingDeps = false;
                SysTime newestDep;
                foreach (dep ; deps)
                {
                    SysTime depTime = Build(dep);
                    if (depTime == SysTime())
                    {
                        missingDeps = true;
                        break;
                    }
                    if (depTime > newestDep)
                    {
                        newestDep = depTime;
                    }
                }

                if (missingDeps)
                {
                    break;
                }

                if ((newestDep > targetTime) || rule.isForce)
                {
                    if (rule.isBuildable)
                    {
                        env.Set("TARGET", [target]);
                        env.Set("DEPS", deps);
                        if (rule.Execute(env))
                        {
                            if (exists(target))
                            {
                                getTimes(target, accessTime, targetTime);
                            }
                            else
                            {
                                targetTime = newestDep;
                            }
                        }
                    }
                    else
                    {
                        targetTime = newestDep;
                    }
                }
                break;
            }
        }

        resolved[target] = targetTime;

        return targetTime;
    }

    bool Post()
    {
        bool rtn = true;

        if (this.postRule !is null)
        {
            rtn = this.postRule.Execute(this.env);
        }

        return rtn;
    }

    private
    {
        class Rule
        {
            this(Posn posn, string target, int flags, Token[] deps, string build)
            {
                this.posn   = posn;
                this.target = target;
                this.flags  = flags;
                this.deps   = deps;
                this.build  = build;
            }

            Tuple!(bool, "matched", string[], "parts") Match(string target)
            {
                return TextUtils.Match(this.target, target);
            }

            string[] Dependents(Enviro env)
            {
                string[] deps;

                foreach (dep ; this.deps)
                {
                    deps ~= Expand(dep, env);
                }

                return deps;
            }

            bool Execute(Enviro env)
            {
                return shell.Execute(this.build, env, this.isQuiet);
            }

            bool isForce()  {return ((this.flags & Flags.FORCE)  != 0) || (this.deps.length == 0);}
            bool isCreate() {return ((this.flags & Flags.CREATE) != 0);}
            bool isTouch()  {return ((this.flags & Flags.TOUCH)  != 0);}
            bool isQuiet()  {return ((this.flags & Flags.QUIET)  != 0);}
            bool isBuildable() {return (this.build !is null) &&(this.build.length != 0);}

            private
            {
                Posn posn;
                string target;
                int flags;
                Token[] deps;
                string build;
            }
        }
        
        class GitBuildRule : Rule
        {
            this(Posn posn, string[] args)
            {
                Token[] deps;
                super(posn, args[0], Flags.CREATE | Flags.FORCE, deps, "git");
                this.target_dir = args[1];
                this.repo       = args[2];
                this.branch     = args[3];
            }

            override bool Execute(Enviro env)
            {
                bool rtn;
                
                if (exists(this.target_dir ~ "/.git"))
                {
                    // Nothing to do. It exists
                    rtn = true;
                }
                else
                {
                    string[] cmd = [Path.FindExe(env.Get("PATH"), "git"), "clone", "-b", this.branch, "--single-branch", "--recurse-submodules", this.repo, this.target_dir];
                    rtn = shell.Execute(cmd);
                }
                
                return rtn;
            }

            private
            {
                string target_dir;
                string repo;
                string branch;
            }
        }
        
        class GitUpdateRule : Rule
        {
            this(Posn posn, string[] args)
            {
                Token[] deps;
                super(posn, args[0]~"-update", Flags.CREATE | Flags.FORCE, deps, "git");
                this.target_dir = args[1];
                this.repo       = args[2];
                this.branch     = args[3];
            }

            override bool Execute(Enviro env)
            {
                bool rtn;
                
                if (exists(this.target_dir ~ "/.git"))
                {
                    string[] here = env.Get("PWD");
                    try
                    {
                        env.Set("PWD", [this.target_dir]);
                        string[] cmd = [Path.FindExe(env.Get("PATH"), "git"), "pull", "origin"];
                        rtn = shell.Execute(cmd);
                    }
                    catch (Exception ex)
                    {
                    }
                    env.Set("PWD", here);
                }
                else
                {
                    // Clone it
                    string[] cmd = [Path.FindExe(env.Get("PATH"), "git"), "clone", "-b", this.branch, "--single-branch", "--recurse-submodules", this.repo, this.target_dir];
                    rtn = shell.Execute(cmd);
                }
                
                return rtn;
            }

            private
            {
                string target_dir;
                string repo;
                string branch;
            }
        }
        
        string FindExe(string[] list ...)
        {
            string tmp = null;

            foreach (entry ; list)
            {
                tmp = Path.FindExe(this.env.Get("PATH"), entry);
                if (tmp !is null)
                {
                    return tmp;
                }
            }

            return "UNKNOWN";
        }

        void DefaultEnv()
        {
            this.env = new Enviro();
        
    		this.env.Set("SIRE",  [thisExePath()]);
    		this.env.Set("CWD",   [getcwd()]);
    		this.env.Set("SHELL", ["sire"]);
    		this.env.Set("SEP",   [pathSeparator]);

            this.env.Set("DC", [FindExe("dmd", "ldc2", "gdc")]); // D compiler

            version(Windows)
            {
                this.env.Set("EXE", [".exe"]);
                this.env.Set("OS", ["WINDOWS"]);
            }
            version(linux)
            {
                this.env.Set("EXE", [""]);
                this.env.Set("OS", ["LINUX"]);
            }
            version(OSX)
            {
                this.env.Set("EXE", [""]);
                this.env.Set("OS", ["MAC"]);
            }
            version(Hurd)
            {
                this.env.Set("EXE", [""]);
                this.env.Set("OS", ["HURD"]);
            }
            version(Cygwin)
            {
                this.env.Set("EXE", [""]);
                this.env.Set("OS", ["CYGWIN"]);
            }
            version(X86)
            {
                this.env.Set("PLATFORM", ["X86"]);
            }
            version(X86_64)
            {
                this.env.Set("PLATFORM", ["X64"]);
            }
            version(ARM)
            {
                this.env.Set("PLATFORM", ["ARM"]);
            }
        }
	  
		void Parse()
		{
			auto token = this.input.GetToken();
			while (token.type != Type.EOF)
			{
				switch(token.type)
				{
					case Type.NAME:
						auto next = this.input.GetToken();
                        switch(next.type)
                        {
                            case Type.NAME:
                            case Type.TEXT:
                                // TargetList
                                ParseRule([token, next]);
                                break;
                                
                            case Type.COLON:
                                this.input.PutToken(next);
                                ParseRule([token]);
                                break;
                                
                            case Type.BLOCK:
                                Token[] deps;
                                AddRule(Expand(token, this.env), 0, deps, next);
                                break;
                                
                            case Type.ASIGN:
                                ParseAssignment(token);
                                break;
                                
                            case Type.OPEN_SQUARE:
                                ParseRuleGen(token);
                                break;

                            default:
                                // Error Illegal token
                                Error(token, "UNEXPECTED : ", token.text);
                                StripToEnd();
                                break;
                        }
                        break;
                    
                    case Type.TEXT:
                        ParseRule([token]);
                        break;
                    
                    case Type.INCLUDE:
                        token = this.input.GetToken();
                        string[] list = Expand(token, this.env);
                        if (list !is null)
                        {
                            EndStatement();
                            if (list.length != 1)
                            {
                                Error(token, "Bad variable expansion ");
                            }
                            else
                            {
                                try
                                {
                                    this.input.Push(token.text, File(list[0]));
                                }
                                catch(Exception ex)
                                {
                                    Error(token, "Error reading file ");
                                }
                            }
                        }
                        break;

                    default:
                        // Error Illegal token
                        Error(token, "UNEXPECTED : ", token.text, ":", to!string(token.type));
                        StripToEnd();
                        break;
                }
                    
                token = this.input.GetToken();
                    
            }
        }
        
        void ParseRule(Token[] targetList)
        {
            int flags = 0;
            Token[] deps;

            string[] targets;
            foreach (target ; targetList)
            {
                targets ~= Expand(target, this.env);
            }
            
            auto token = this.input.GetToken();
            while ((token.type == Type.NAME) ||
                   (token.type == Type.TEXT))
            {
                targets ~= Expand(token, this.env);
                token = this.input.GetToken();
            }

            if (token.type == Type.BLOCK)
            {
                AddRule(targets, flags, deps, token);
            }
            else if (token.type == Type.COLON)
            {
                token = this.input.GetToken();
                
                if ((token.type == Type.CREATE)||
                    (token.type == Type.TOUCH) ||
                    (token.type == Type.FORCE) ||
                    (token.type == Type.QUIET))
                {
                    do
                    {
                        switch(token.type)
                        {
                            case Type.CREATE: flags |= Flags.CREATE; break;
                            case Type.TOUCH:  flags |= Flags.TOUCH; break;
                            case Type.FORCE:  flags |= Flags.FORCE; break;
                            case Type.QUIET:  flags |= Flags.QUIET; break;
                            default: assert(false);
                        }
                        
                        token = this.input.GetToken();
                    }
                    while ((token.type == Type.CREATE)||
                           (token.type == Type.TOUCH) ||
                           (token.type == Type.FORCE) ||
                           (token.type == Type.QUIET));

                    if (token.type != Type.COLON)
                    {
                        Error(token, "Bad rule");
                        return;
                    }

                    token = this.input.GetToken();
                }

                while ((token.type == Type.NAME) ||
                       (token.type == Type.TEXT) ||
                       (token.type == Type.FILES))
                {
                    deps ~= token;
                    token = this.input.GetToken();
                }

                if (token.type == Type.SEMI_COLON)
                {
                    // Create an empty block
                    token.type = Type.BLOCK;
                    token.text = "";
                }

                if (token.type == Type.BLOCK)
                {
                    AddRule(targets, flags, deps, token);
                }
            }
            else
            {
                Error(targetList[0], "Bad rule");
                StripToEnd();
            }
        }
        
        void AddRule(string[] targetList, uint flags, Token[] depsList, Token block)
        {
            if ((flags & (Flags.CREATE | Flags.TOUCH)) == 0)
            {
                // Default flags
                flags |= Flags.CREATE | Flags.TOUCH;
            }
            
            foreach (target ; targetList)
            {
                // Create rule
                if (target == "pre")
                {
                    if (this.preRule is null)
                    {
                        this.preRule = new Rule(block.posn, target, flags, depsList, block.text); 
                    }
                    else
                    {
                        Error(block, "Duplicate preamble");
                    }
                }
                else if (target == "post")
                {
                    if (this.postRule is null)
                    {
                        this.postRule = new Rule(block.posn, target, flags, depsList, block.text); 
                    }
                    else
                    {
                        Error(block, "Duplicate postamble");
                    }
                }
                else
                {
                    this.rules ~= new Rule(block.posn, target, flags, depsList, block.text); 
                }
            }
        }
        
        void ParseAssignment(Token var)
        {
            string[] list;
            
            auto token = this.input.GetToken();
            while ((token.type == Type.NAME) ||
                   (token.type == Type.TEXT) ||
                   (token.type == Type.FILES) ||
                   (token.type == Type.EXE_FILE))
            {
                list ~= Expand(token, this.env);
                token = this.input.GetToken();
            } 
            
            if (token.type == Type.COLON)
            {
                auto match1 = this.input.GetToken();
                if (match1.type != Type.TEXT)
                {
                    Error(match1, "Missing match");
                    this.input.PutToken(match1);
                    StripToEnd();
                    return;
                }
                
                token = this.input.GetToken();
                if (token.type != Type.COLON)
                {
                    Error(token, "Missing :");
                    this.input.PutToken(token);
                    StripToEnd();
                    return;
                }
                
                auto match2 = this.input.GetToken();
                if (match1.type != Type.TEXT)
                {
                    Error(match1, "Missing match");
                    this.input.PutToken(match2);
                    StripToEnd();
                    return;
                }

                // Remap the entries
                string[] list2;
                foreach (entry ; list)
                {
                    auto result = Match(match1.text, entry);
                    if (result.matched)
                    {
                        list2 ~= ExpandVar(match2.text, new Enviro(this.env, true, result.parts));
                    }
                }
                list = list2;
                
                token = this.input.GetToken();
            }
            
            if (token.type != Type.SEMI_COLON)
            {
                Error(token, "Missing ;");
                StripToEnd();
            }

            this.env.Set(var.text, list);
        }
        
        void ParseRuleGen(Token var)
        {
            string[] list;
            
            auto token = this.input.GetToken();
            Posn posn = token.posn;
            while ((token.type == Type.NAME) ||
                   (token.type == Type.TEXT) ||
                   (token.type == Type.EXE_FILE))
            {
                list ~= Expand(token, this.env);
                token = this.input.GetToken();
            }

            if (token.type != Type.CLOSE_SQUARE)
            {
                Error(token, "Missing ]");
                StripToEnd();
            }
            else
            {
                token = this.input.GetToken();
                if (token.type != Type.SEMI_COLON)
                {
                    Error(token, "Missing ;");
                    StripToEnd();
                }
            }

            switch (var.text)
            {
                case "GIT":
                    if (list.length != 4)
                    {
                        Error(var, "Incorrent arguments to GIT");
                    }
                    else
                    {
                        // Build rules
                        this.rules ~= new GitBuildRule(posn, list);
                        this.rules ~= new GitUpdateRule(posn, list);
                    }
                    break;

                default:
                    Error(var, "unknown command ", var.text);
                    break;
            }
        }
                      
		
		bool EndStatement()
		{
			bool valid = true;
			auto token = this.input.GetToken();
			if (token.type != Type.SEMI_COLON)
			{
				valid = false;
				Error(token, "Missing semi-colon ");
				while ((token.type != Type.SEMI_COLON) &&
				       (token.type != Type.EOF))
				{
					token = this.input.GetToken();
				}
			}
			
			return valid;
		}

        void StripToEnd()
        {
            auto token = this.input.GetToken();
            while ((token.type != Type.SEMI_COLON) &&
                   (token.type != Type.EOF))
            {
                token = this.input.GetToken();
            }
        }
		
		void Error(Token token, string[] text ...)
		{
			writeln(token.posn, text);
		}

        Rule   preRule;
        Rule[] rules;
        Rule   postRule;
        
        SysTime[string] resolved;
        
		InputStack input;

        Enviro env;
	}
}

	
private
{
	enum Type
	{
		NONE,
		EOF,
		BLOCK,
		OPEN_SQUARE,
		CLOSE_SQUARE,
		COLON,
		SEMI_COLON,
		ASIGN,
		TOUCH,
		CREATE,
        FORCE,
		QUIET,
		INCLUDE,
		NAME,
		TEXT,   // General text
		EXE_FILE,
		FILES   // Text expandable to a set of files in the file system
	}

	struct Token
	{
		this(string text, Type type)
		{
			this.text = text;
			this.type = type;
		}
		
		string text;
		Type   type;
		Posn   posn;
	}
	
	struct Posn
	{
		string name = "";
		uint   line = 0;
		uint   column = 0;
		
		string toString()
		{
			return format("[%s:%d:%d] ", this.name, this.line, this.column);
		}
	}

    string[] Expand(Token token, Enviro env)
    {
        switch (token.type)
        {
            case Type.TOUCH:
            case Type.CREATE:
            case Type.FORCE:
            case Type.QUIET:
                return [token.text];
                
            case Type.BLOCK:
            case Type.NAME:
            case Type.TEXT:
                return ExpandVar(token.text, env);
                
            case Type.EXE_FILE:
                auto list = ExpandVar(token.text, env);
                if (list.length != 1)
                {
                    writeln(token.posn, "Not expandable ", token.text);
                    break;
                }
                else
                {
                    return [FindExe(env.Get("PATH"), list[0].idup)];
                }
                
            case Type.FILES:   // Text expandable to a set of files in the file system
                string[] list;
                foreach (match ; ExpandVar(token.text, env))
                {
                    list ~= FindFiles(match);
                }
                return list;
                    
            default:
                throw new SireException(to!string(token.posn) ~ "Not expandable " ~ token.text);
                break;
        }

        return ["UNKNOWN"];
    }

	class InputStack
	{
		void Push(string name, File fp)
		{
			this.head = new Node(new Tokeniser(fp, name), this.head);
		}
		
		Token GetToken()
		{
			string name = "<EOF>";
			Token token;
			token.text = "<EOF>";
			token.type = Type.EOF;
			
			while ((this.head !is null) && (token.type == Type.EOF))
			{
				token = this.head.tokenise.GetToken();
				
				if (token.type == Type.EOF)
				{
					this.head = this.head.prev;
				}
			}
			
			return token;
		}
		
		Token PutToken(Token token)
		{
			if ((this.head !is null) && (token.type != Type.EOF))
			{
				this.head.tokenise.PutToken(token);
			}
			
			return token;
		}
		
		class Node
		{
			this(Tokeniser tokenise, Node prev)
			{
				this.tokenise = tokenise;
				this.prev     = prev;
			}
			
			Tokeniser   tokenise;
			Node        prev;
		}
		
		Node head;
	}
		
	class Tokeniser
	{
		enum State
		{
			WHITE,
			NAME,
			QUOTED_TEXT,
			TEXT,
			FILES,
			EXE_FILE,
			BLOCK,
			COMMENT1,
			COMMENT2
		}

		this(File fp, string name)
		{
			this.fp = fp;
			this.filename = name;
		}
		
		bool isNameChar(char ch)
		{
			return 
			   (isAlphaNum(ch) || 
				(ch == '_')
			    );
		}
		
		bool isTextChar(char ch)
		{
			return 
			   (isAlphaNum(ch) || 
				(ch == '_') || 
				(ch == '-') || 
				(ch == '\\') || 
				(ch == '/') || 
				(ch == '.') || 
                
				(ch == '?') || 
				(ch == '*') || 
				
				(ch == '$') ||  // Variable expansion
				(ch == '{') ||
				(ch == '}')
			    );
		}
		
        void PutToken(Token token)
        {
            // Push the token (one deep)
            lastToken = token;
        }
		
		Token GetToken()
		{
			uint depth = 0;
			bool wild = false;
			Posn posn;

            // Pop off the last pushed token.
            if (lastToken.type != Type.NONE)
            {
                Token token = lastToken;
                lastToken.type = Type.NONE;
                return token;
            }
			
			char ch;
			while (GetChar(ch))
			{
				switch(this.state)
				{
					case State.WHITE:
						
						if (isWhite(ch))
						{
							// Discard
						}
						else
						{
							if (ch == '/')
							{
								posn = GetPosn();
								this.state = State.COMMENT1;
							}
							else if (ch == '\"')
							{
								posn = GetPosn();
								this.state = State.QUOTED_TEXT;
							}
							else if (ch == '\'')
							{
								posn = GetPosn();
								this.state = State.FILES;
							}
							else if (ch == '<')
							{
								posn = GetPosn();
								this.state = State.EXE_FILE;
							}
							else if (ch == '{')
							{
								depth = 1;
								posn = GetPosn();
								this.state = State.BLOCK;
							}
							else if (ch == '[')
							{
								Push(ch);
								return ReturnToken(Type.OPEN_SQUARE, GetPosn());
							}
							else if (ch == ']')
							{
								Push(ch);
								return ReturnToken(Type.CLOSE_SQUARE, GetPosn());
							}
							else if (ch == ':')
							{
								Push(ch);
								return ReturnToken(Type.COLON, GetPosn());
							}
							else if (ch == ';')
							{
								Push(ch);
								return ReturnToken(Type.SEMI_COLON, GetPosn());
							}
							else if (ch == '=')
							{
								Push(ch);
								return ReturnToken(Type.ASIGN, GetPosn());
							}
							else if (isNameChar(ch))
							{
								Push(ch);
								posn = GetPosn();
								this.state = State.NAME;
							}
							else if (isTextChar(ch))
							{
								Push(ch);
								posn = GetPosn();
								this.state = State.TEXT;
							}
							else
							{
								Error(posn);
							}
						}
						break;
						
					case State.NAME:
						if (isWhite(ch))
						{
							this.state = State.WHITE;
							
							return ReturnNameToken(posn);
						}
						else if (isNameChar(ch))
						{
							Push(ch);
						}
						else if (isTextChar(ch))
						{
							this.state = State.TEXT;
							Push(ch);
						}
						else
						{
							PutChar(ch);
							this.state = State.WHITE;
							
							return ReturnNameToken(posn);
						}
						break;
						
					case State.TEXT:
						if (!isTextChar(ch))
						{
							PutChar(ch);
							this.state = State.WHITE;
							
							return ReturnToken(Type.TEXT, posn);
						}
						else
						{
							Push(ch);
						}
						break;
						
					case State.QUOTED_TEXT:
						if (ch == '\"')
						{
							this.state = State.WHITE;
							
							return ReturnToken(Type.TEXT, posn);
						}
						else if ((ch == '\r') || (ch == '\n'))
						{
							// No end of quoted text
							this.state = State.WHITE;
							Error(posn);
							return ReturnToken(Type.TEXT, posn);
						}
						else
						{
							Push(ch);
						}
						break;
						
					case State.FILES:
						if (ch == '\'')
						{
							this.state = State.WHITE;
							
							return ReturnToken(Type.FILES, posn);
						}
						else if ((ch == '\r') || (ch == '\n'))
						{
							// No end of expand name
							this.state = State.WHITE;
							Error(posn);
							return ReturnToken(Type.FILES, posn);
						}
						else
						{
							Push(ch);
						}
						break;
						
					case State.EXE_FILE:
						if (ch == '>')
						{
							this.state = State.WHITE;
							
							return ReturnToken(Type.EXE_FILE, posn);
						}
						else if ((ch == '\r') || (ch == '\n'))
						{
							// No end of expand name
							this.state = State.WHITE;
							Error(posn);
							return ReturnToken(Type.EXE_FILE, posn);
						}
						else
						{
							Push(ch);
						}
						break;
						
					case State.BLOCK:
						if (ch == '{')
						{
							Push(ch);
							depth += 1;
						}
						else if (ch == '}')
						{
							depth -= 1;
							
							if (depth == 0)
							{
								this.state = State.WHITE;
								return ReturnToken(Type.BLOCK, posn);
							}
							else
							{
								Push(ch);
							}
						}
						else
						{
							Push(ch);
						}
						break;
						
					case State.COMMENT1:
						if (isWhite(ch) || (ch == '\"') || (ch == '\'') || (ch == '{'))
						{
							PutChar(ch);
							this.state = State.WHITE;
							
							Push('/');
							return ReturnToken(Type.NAME, posn);
						}
						else if (ch == '/')
						{
							this.state = State.COMMENT2;
						}
						else if (isPrintable(ch))
						{
							Push('/');
							Push(ch);
							this.state = State.NAME;
						}
						else
						{
							Error(posn);
						}
						break;
						
					case State.COMMENT2:
						if (ch == '\r')
						{
							this.state = State.WHITE;
							buffer.clear();
						}
						else if (ch == '\n')
						{
							this.state = State.WHITE;
							buffer.clear();
						}
						else
						{
							// Dicard comment
							Push(ch);
						}
						break;
						
					default: assert(0);
				}
			}
			
			if (state == State.BLOCK)
			{
				// No end of BLOCK
				this.state = State.WHITE;
				Error(posn);
				return ReturnToken(Type.BLOCK, posn);
			}
			
			if (state == State.QUOTED_TEXT)
			{
				// No end of quoted name
				this.state = State.WHITE;
				Error(posn);
				return ReturnToken(Type.TEXT, posn);
			}
			
			if (state == State.FILES)
			{
				// No end of expanded name
				this.state = State.WHITE;
				Error(posn);
				return ReturnToken(Type.FILES, posn);
			}
			
			if (state == State.NAME)
			{
				this.state = State.WHITE;
				return ReturnToken(Type.NAME, posn);
			}
			
			return Token("<EOF>", Type.EOF);
		}
		
		char GetChar(ref char ch)
		{
			if (this.lastChar != '\0')
			{
				ch = this.lastChar;
				this.lastChar = '\0';
				return true;
			}
			else
			{
				bool ok = (this.fp.readf!"%c"(ch) == 1);
				
				if (ch == '\n')
				{
					if (last != '\r')
					{
						this.line += 1;
						this.column = 0;
					}
				}
				else if (ch == '\r')
				{
					if (last != '\n')
					{
						this.line += 1;
						this.column = 0;
					}
				}
				else 
				{
					this.column += 1;
				}
				
				last = ch;
				
				return ok;
			}
		}
		
		void PutChar(char ch)
		{
			if (this.lastChar != '\0')
			{
				assert(0);
			}
			else
			{
				this.lastChar = ch;
			}
		}
			
		void Push(char ch)
		{
			buffer.put(ch);
		}
		
		Token ReturnToken(Type type, Posn posn)
		{
			// Return a name token
			Token token;
			token.text = buffer.data().idup;
			token.type = type;
			token.posn = posn;
			buffer.clear();
			
			return token;
		}
		
		Token ReturnNameToken(Posn posn)
		{
			// Return a name token
			
			Token token;
			token.text = buffer.data().idup;
			token.posn = posn;
			buffer.clear();
			
			switch (token.text)
			{
				case "TOUCH"   : token.type = Type.TOUCH;   break;
				case "CREATE"  : token.type = Type.CREATE;  break;
				case "FORCE"   : token.type = Type.FORCE;   break;
				case "QUIET"   : token.type = Type.QUIET;   break;
				case "INCLUDE" : token.type = Type.INCLUDE; break;
				default: token.type = Type.NAME; break;
			}
			
			return token;
		}
		
		Posn GetPosn()
		{
			return Posn(this.filename, line, column);
		}
		
		void Error(Posn posn, string[] args ...)
		{
			writeln(posn, args);
			buffer.clear();
		}

        Token lastToken = Token("<NONE>", Type.NONE);
		
		State state = State.WHITE;
		auto buffer = appender!(char[]);
		char lastChar = '\0';
		File fp;
		
		char last = 1;
		int line = 1;
		int column = 0;
		string filename = "<UNKNOWN>";
	};

}
