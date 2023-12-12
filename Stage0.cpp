//Jonathan Morgan && Carlos Cardenas
// Stage 0
// 4301

#include <stage0.h>
#include <iostream>
#include <ctime>
#include <string>
#include <cctype>
#include <iomanip>
#include <vector>
#include <fstream>

using namespace std;

static bool hasErrorBeenFound = false;

Compiler::Compiler(char **argv) //constructor
{
	//open 3 sourcefiles with argv
	sourceFile.open(argv[1]);
	listingFile.open(argv[2]);
	objectFile.open(argv[3]);
	
}

Compiler::~Compiler() //destructor
{
	//close 3 files
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}

void Compiler::createListingHeader()
{
	time_t now = time (NULL);
	
	listingFile << "STAGE0:   JONATHAN MORGAN & CARLOS CARDENAS         ";
	listingFile << ctime(&now) << "\n";
	listingFile << "LINE NO.			SOURCE STATEMENT" <<  "\n\n";
	
}

void Compiler::parser()
{
	nextChar();

	if (nextToken() != "program")
		processError("keyword \"program\" expected");
			
	prog();
	
}

void Compiler::createListingTrailer()
{
	//ofstream listingFile;
	
	if(hasErrorBeenFound == true)
		listingFile <<"COMPILATION TERMINATED      " << "1 ERRORS ENCOUNTERED\n";
	
	else listingFile << "COMPILATION TERMINATED      " << "0 ERRORS ENCOUNTERED\n";
}

void Compiler::processError(string err)
{
	listingFile << '\n' << "Error: Line " << lineNo << ": " << err << "\n\n";
	
	hasErrorBeenFound = true;
	createListingTrailer();
	
	//exit(0);
}

	// Grammar Rules
	
	//Production 1
	//token should be program
void Compiler::prog()
{
	//cout << "end processError" << endl;
	
	if(token != "program")
	{
		processError("keyword \"program\" expected");
	}
	
	//cout << "end processError" << endl;

	progStmt();
	
	if(token == "const")
	{
		consts();
	}
		
	if(token == "var")
	{
		vars();
	}
		
	if(token != "begin")
	{
		processError("keyword \"begin\" expected");
	}
	
	beginEndStmt();
	
	if(token != "$") // may be wrong
	{
		processError("no text may follow \"end\"");
	}


}

//Production 2
//token should be program
void Compiler::progStmt()
{
	string x;
	
	if(token != "program")
	{
		processError("keyword \"program\" expected");
	}
	
	x = nextToken();
	
	if(isNonKeyId(token) == false)
	{
		processError("program name expected");
	}
	
	
	if(nextToken() != ";")
	{
		processError("semicolon expected");
	}
	
	nextToken();
	
	code("program", x);
	
	insert(x, PROG_NAME, CONSTANT, x, NO, 0);
}

//Production 3
//token should be "const"
void Compiler::consts()
{
	if(token != "const")
	{
		processError("keyword \"const\" expected");
	}
	
	if(isNonKeyId(nextToken()) == false)
	{
		processError("non-keyword identifier must follow \"const\"");
	}
	
	constStmts();
}

//Production 4
//token should be "var"
void Compiler::vars()
{
	if(token != "var")
	{
		processError("keyword \"var\" expected");
	}
	
	if(isNonKeyId(nextToken()) == false)
	{
		processError("non-keyword identifier must follow \"var\"");
	}	
	varStmts();
}

//Production 5
//token should be "begin"
void Compiler::beginEndStmt()
{
	if(token != "begin")
	{
		processError("keyword \"begin\" expected");
	}
	
	if(nextToken() != "end")
	{
		processError("keyword \"end\" expected");
	}
	
	if(nextToken() != ".")
	{
		processError("period expected");
	}
	
	nextToken();	
	code("end", ".");
}

//Production 6
//token should be NON_KEY_ID
void Compiler::constStmts()
{
	string x,y;
	int num;
	
	if(isNonKeyId(token) == false)
	{
		processError("non-keyword identifier expected");
	}
	
	x = token;
	
	if(nextToken() != "=")
	{
		processError("\"=\" expected");
	}
	
	y = nextToken();
	
	if(!(isNonKeyId(y)) && !(isInteger(y)) && ((y != "-") && (y != "+") && (y != "not")) && !(isBoolean(y)))
	{
		listingFile << " token: " << y << endl;
		processError("token to right of \"=\" illegal");
		
	}
	
	if((y == "+") || (y == "-"))
	{
		if(isInteger(nextToken()) == false)
		{
			processError("integer expected after sign");
		}
		
		if( y == "+")
		{
			y = "";
		}
		
		y += token;
	}
	
	if(isBoolean(y))
	{
		if(y == "true")	
			num = -1;
		
		else num = 0;
	}
	
	if(y == "not")
	{
		if(isBoolean(nextToken()) == false)
		{
			processError("boolean expected after \"not\"");
		}
		else if(token == "true")
		{
			y = "false";
			num = 0;
		}
		else
		{
			y = "true";
			num = -1;
		}
	}
	
	if(nextToken() != ";")
		processError("semicolon expected");
	
	if(whichType(y) != INTEGER && whichType(y) != BOOLEAN)
		processError("data type of token on the right-hand side muse be INTEGER or BOOLEAN");
	if(whichType(y) == INTEGER && y != "+" && y != "-")
	{
		num = static_cast<int>(y[0] - 48);
		//cout << y << " the interpeted num: " << num << endl;
	}
	
	else if(whichType(y) == BOOLEAN)
	{
		if(y == "true")
		{
			num = -1;
		}
		else num = 0;
	}
	
	insert(x,whichType(y),CONSTANT,whichValue(y),YES,num);
	x = nextToken();
	if((x != "begin" && x != "var") && isNonKeyId(x) == false)
	{
		processError("non-keyword identifier, \"begin\", or \"var\" expected");
	}
	
	if(isNonKeyId(x) == true)
		constStmts();	
}

//Production 7
//Token should be NON_KEY_ID
void Compiler::varStmts()
{
	string x, y;
	
	if(isNonKeyId(token) == false)
	{
		processError("non-keyword identifier expected");
	}
	
	x = ids();
	
	if(token != ":")
		processError("\":\" expected");
	
	nextToken();
	
	if(isInteger(token) == false && isBoolean(token) == false)
		processError("illegal type follows \":\"");
	
	y = token;
	
	if(nextToken() != ";")
		processError("semicolon expected");
	
	insert(x,whichType(y),VARIABLE,"",YES,1);
	
	nextToken();
	
	if(token != "begin" && isNonKeyId(token) == false)
		processError("non-keyword identifier or \"begin\"  expected");
	
	if(isNonKeyId(token) == true)
		varStmts();
}
//returns the next token or end of file marker
string Compiler::nextToken()
{
	token = "";
	while(token == "")
	{
		if (ch == '{') 
		{
			while(nextChar() != END_OF_FILE && ch != '}')
			{}
			
			if (ch == END_OF_FILE)
			{
				processError("unexpected end of file");
			}
			//if (ch == '}')
			else
			{
				nextChar();
			}
		}
		
		else if (ch == '}')
		{
			processError("'}' cannot begin token");
		}
		
		else if (isspace(ch))
		{
			nextChar();
		}
		
		else if (isSpecialSymbol(ch))
		{
			token = ch;
			nextChar();
		}
		
		else if (islower(ch))
		{
			token = ch;
			nextChar();
			char lastCh = ' ';
			while(((ch == '_') || (isalpha(ch)) || (isdigit(ch))) && (ch != END_OF_FILE))
			{
				lastCh = ch;
				token += ch;
				nextChar();
			}
			
			if(lastCh == END_OF_FILE)
			{
				processError("unexpected end to file");
			}
			
		}
		
		else if(isdigit(ch)) 
		{
			token = ch;
			nextChar();
			while((isdigit(ch)) && (ch != END_OF_FILE))
			{
				token += ch;
				nextChar();
			}
			
			if(ch == END_OF_FILE)
			{
				processError("unexpected end of file");
			}
		}
		
		else if(ch == END_OF_FILE)
		{
			token = ch;
		}
		
		else
		{
			processError("illegal symbol");
		}
		//finish function
	}
		
	return token;
}

//returns the next character or end of file marker
char Compiler::nextChar()
{
	char nextCh;
	static char lastCh = '\n'; // test entry
	sourceFile.get(nextCh);
	
	//check if it's the end of the file
	if(sourceFile.eof())
	{
		ch = END_OF_FILE;
	}
	
	else
	{
		lastCh = ch;		
		ch = nextCh;
	}
	
	if(lineNo == 0) //test entry
	{
		lineNo++;
		listingFile << setw(5) << lineNo << "|";
	}		
	
	else if(lastCh == '\n')
	{
		lineNo++;
		listingFile << setw(5) << lineNo << "|";
	}
	
	listingFile << ch;
	
	return ch;
}

bool Compiler::isNonKeyId(string s) const
{
	if((islower(s[0]) == false) || isKeyword(s))	//if first letter of s is lowercase
		return false;								//or s is keyword it isn't a keyID
	
	for(uint i = 0; i < s.length(); ++i)
	{
		if((isdigit(s[i]) == false) && (islower(s[i]) == false) && (s[i] != '_'))
			return false;	//is s constains a digit and lowercase and s doesnt contain '_' it isn't a keyID
	}
	
	if(s.length() >= 15) //check if 15th char is '_'
	{
		if(s[14] == '_')
			return false;
	}
	
	int ct = 0;
	
	//checks for special symbols 
	for(uint i = 0; i < s.length(); ++i){
		if(isSpecialSymbol(s[i])){
			if(s[i] == '_' && ct < 1){ //nonKeyID can have one '_'
				++ct;
			}
			else{
				return false;
	}}}
	
	return true;
}

bool Compiler::isInteger(string s) const
{
	if(isdigit(s[0]))
	{
		for (uint i = 0; i < s.length(); ++i)
		{
			if(!isdigit(s[i]))
			{
				return false;
			}
		}
		return true;
	}
	else if(s == "integer")
		return true;
	
	else if(((s[0] == '-') || (s[0] == '+')) && isdigit(s[1]))
	{
			return true;
	}
	
	return false;
}

void Compiler::code(string op, string operand1, string operand2)
{
	if(op == "program")
		emitPrologue(operand1);
	
	else if(op == "end")
		emitEpilogue();
	
	else
		processError("compiler error since function code should not be called illegal arguments");
}

void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
	if (inType == PROG_NAME)
		return;
	
	string name, jic;
		name = "";
		jic = "";
		
		for(uint i = 0; i < externalName.length(); ++i)
		{
			if(externalName[i] != ',')
			{
				name += externalName[i];
			}
			
			else
			{
				for(uint j = i + 1; j < externalName.length(); ++j)
					jic += externalName[j];
				
				break;
			}
		}		
		//cout << name << " " << jic << endl;
	
	if(symbolTable.count(name) > 0)
		processError("multiple name definition");
	
	else if(isKeyword(name) && name != "true" && name != "false")
		processError("illegal use of keyword");
	
	else
	{
					
		if(isupper(name[0]))
			symbolTable.insert ({name, SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)});
		
		else
			symbolTable.insert ({name, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)});
	}
	
	if(jic != "")
		{
			if(symbolTable.count(jic) > 0)
				processError("multiple name definition");
	
			else if(isKeyword(jic) && jic != "true" && jic != "false")
				processError("illegal use of keyword");
			
			else
			{
				if(isupper(jic[0]))
				{
					symbolTable.insert ({jic, SymbolTableEntry(jic, inType, inMode, inValue, inAlloc, inUnits)});
				}
				
				else
				{
					symbolTable.insert ({jic, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)});
				}
			}
		}
}

bool Compiler::isBoolean(string s) const
{
	if(s == "true" || s == "false")
		return true;
	
	if(s == "boolean")
		return true;
	
	return false;
}

string Compiler::whichValue(string name)
{
	string value = "temp";
	
	if(isLiteral(name))
	{
		if(isBoolean(name))
			value = name;
		
		else if(isdigit(name[0]))
		{
			value = name[0];
			uint i = 1;
			
			while (i < name.length())
			{
				value += name[i];
				++i;
			}
		}
		else if(name[0] == '+' || name[0] == '-')
		{
			if(name[0] == '-')
			{
				value = name[0];
				uint i = 1;
			
				while (i < name.length())
				{
					value += name[i];
					++i;
				}
			}
			else
			{
				uint i = 1;
			
				while (i < name.length())
				{
					value += name[i];
					++i;
				}
			}
		}
	}

	else
	{
		auto it = symbolTable.find(name);
		
		if(symbolTable.count(name) == 1)
			value = (*it).second.getValue();
		else
		{
			processError("reference to undefined constant");
			//listingFile << "here " << name;
		}
	}
	
	return value;
}

storeTypes Compiler::whichType(string name)
{
	storeTypes dataType;
	
	if(isInteger(name))
	{
		dataType = INTEGER;
		return dataType;
	}
	else if(isBoolean(name))
	{
		dataType = BOOLEAN;
	}
	
	else
	{
		if(symbolTable.count(name) == 1)
		{
			dataType = symbolTable.at(name).getDataType();
		}
		
		else
		{
			listingFile << endl << "name in question: " << name << endl;
			processError("reference to undefined constant");
			
		}
	}
			
	return dataType;
}

string Compiler::ids()
{
	string s, tempS;
	
	if(!isNonKeyId(token))
		processError("non-keyword identifier expected");
	
	tempS = token;
	s = token;
	
	if(nextToken() == ",")
	{
		if(!isNonKeyId(nextToken()))
			processError("non-keyword identifier expected");
		
		tempS = s + "," + ids();
	}
	
	return tempS;
}

bool Compiler::isSpecialSymbol(char c) const
{
	vector<char> symb {'=', ':', ',', '.', '+', '-', ';'};
	
	for(uint i = 0; i < symb.size(); ++i)
	{
		if(c == symb[i])
			return true;
	}
	
	return false;
}

bool Compiler::isKeyword(string s) const
{
	vector<string> keys {"program", "begin", "end", "var", "const", "integer", "boolean", "true", "false", "not"};
	
	for(uint i = 0; i < keys.size(); ++i)
	{
		if(s == keys[i])
			return true;
	}
	
	return false;
}

bool Compiler::isLiteral(string s)	const
{
	if(isInteger(s) || isBoolean(s))
		return true;
	else if(s == "not" || s == "+" || s == "-")
		return true;
	
	return false;
}

void Compiler::emit(string label, string instruction, string operands, string comment)
{
	objectFile << left << setw(8) << label;
	objectFile << left << setw(8) << instruction;
	objectFile << left << setw(24) << operands << left << comment << "\n";
}

void Compiler::emitPrologue(string progName, string s)
{
	time_t result = time(nullptr);
	objectFile << "; JONATHAN MORGAN, CARLOS CARDENAS " << ctime(&result);
	objectFile << "%INCLUDE \"Along32.inc\"\n" << "%INCLUDE \"Macros_Along.inc\"\n" << endl;
	
	emit("SECTION", ".text");
	emit("global", "_start", "", "; program " + progName);
	
	objectFile << "\n";
	
	emit("_start:");
}

void Compiler::emitEpilogue(string s1, string s2)
{
	emit("", "Exit", "{0}", "");
	objectFile << endl;
	emitStorage();
}

void Compiler::emitStorage()
{
	emit("SECTION", ".data", "", "");
	
	
	for(auto it = symbolTable.cbegin(); it != symbolTable.cend(); ++it)
	{
		//if((*it).second.getValue() != NULL)
		//{
			string val = "";
			
			if(((*it).second.getAlloc() == YES) && ((*it).second.getMode() == CONSTANT))
			{
				if((*it).second.getValue() == "true")
				{
					val = "-1";
				}
				else if((*it).second.getValue() == "false")
					val = "0";
				
				else if((*it).second.getDataType() == INTEGER)
				{
					val = (*it).second.getValue();
				}
					
				emit((*it).second.getInternalName(), "dd", val, ("; "+(*it).first));
			}
			else continue;
			/* else if(((*it).second.getAlloc() == YES) && ((*it).second.getMode() == CONSTANT))
			{
				emit((*it).second.getInternalName(), "dd", val , ("; "+(*it).first));
			} */
			/* else
				emit((*it).second.getInternalName(), "dd", (*it).second.getValue(), ("; "+(*it).first)); */
	}//}
	
	objectFile << "\n";
	emit("SECTION", ".bss", "", "");
	
	for(auto it = symbolTable.cbegin(); it != symbolTable.cend(); ++it)
	{	
		if(((*it).second.getAlloc() == YES) && ((*it).second.getMode() == VARIABLE))
			emit((*it).second.getInternalName(), "resd", "1", ("; "+(*it).first));
	}
}

static int boolCt;
static int intCt;
string Compiler::genInternalName(storeTypes stype) const
{
	
	string internalName;
	
	if(stype == INTEGER)
	{
		internalName = "I" + to_string(intCt);
		++intCt; 
	}
	
	else if(stype == BOOLEAN)
	{
		internalName = "B" + to_string(boolCt);
		++boolCt;
	}
	
	else internalName = "und";
		
	return internalName;
}
