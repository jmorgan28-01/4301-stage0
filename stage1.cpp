//Jonathan Morgan && Carlos Cardenas
// 4301
// Stage 1
//Did Recieve Help From Joel!

#include <stage1.h>
#include <iostream>
#include <ctime>
#include <string>
#include <cctype>
#include <iomanip>
#include <vector>
#include <fstream>

using namespace std;

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
	listingFile << "LINE NO.			SOURCE STATEMENT" <<  "\n" << endl;
	listingFile << flush;
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
	
		listingFile << "COMPILATION TERMINATED      " << "0 ERRORS ENCOUNTERED" << endl;
}

// Methods implementing the grammar productions
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
    string error;

    if (token != "begin")
    {
		error = "keyword \"begin\" expected";
		processError(error);
    }

    nextToken();
	execStmts();	
	

    if (token != "end")
    {
		//cout << endl << token << endl;
		error =  "keyword \"end\" expected";
		processError(error);
    }
	
	nextToken();
	
    if (token == ".") 
    {
		code("end", ".");
		token[0] = '$';
    }

	else
	{
		processError("'.' or ';' expected following \"end\"");
	}
	
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
		/* nextToken();
		y += token; */

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
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//Production 8
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

void Compiler::execStmts()    
{
	if (token == "begin")
	{
		beginEndStmt();
	}
	
	else if (isNonKeyId(token) || token == "read" || token == "write" || token == "if" || token == "while" || token == "repeat" || token == ";")
	{
		execStmt();	
	}

	else if (token == "end");
	
	else
	{
		//cout << "error execStmts" << endl;
		processError("one of \"read\", \"write\", or keyword expected");		//error here
	}
}
void Compiler::execStmt()      
{	
	if (token == "end")
	{
		return;
	}
    else if (token == "read")
    {
      readStmt();
	  nextToken();
	  execStmts();
    }
    else if (token == "write")
    {
      writeStmt();
	  nextToken();
	  execStmts();
    }
	
	else if (isNonKeyId(token))
    {
      assignStmt();
	  nextToken();
	  execStmts();
    }
	
	
    else
    {
		//cout << "error execStmt " << token << endl;
      processError("non-keyword id, \"read\", or \"write\" expected ");
    }
}

void Compiler::assignStmt()    
{
    string first, second;

    if (!isNonKeyId(token))
    {
      processError("non - keyword identifier expected");
    }
  
    if (symbolTable.count(token) == 0)	//if the token is not defined in our symbol table
    {
		//cout << token << endl;
	  processError("reference to undefined variable");
    }
	//cout << token << endl;
    pushOperand(token);
	second = popOperand();
	nextToken();
	//cout << token << endl;

	if (token != ":=")
	{
		processError("':=' expected; found " + token);
	}

	pushOperator(":=");	//push the operator ":=" onto the stack
	nextToken();
	
  
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token) && token != ";")
	{
		processError("expected non_key_id, integer, \"not\", \"true\", \"false\", '(', '+', or '-'");
	}
	
	//cout << token << endl;
	pushOperand(token);
	express();
  
	
	//cout << second << endl;
	first = popOperand();

	//cout << "$$" << second << " | " << first << "$$" << endl;
	code(popOperator(), second, first); //check this line
	//cout << "$$" << second << " | " << first << "$$" << endl;

}

void Compiler::readStmt()      
{
	string x;
	if (token != "read")
	{	
		processError("read expected; found " + token);
	}

	nextToken();

	if (token != "(")
	{
		processError("'(' expected; found " + token);
	}

	nextToken();
	x = ids();
	string w = "";
	for(char& d : x)
	{
		if(d == ',')
		{
			code("read", w);
			w = "";
		}
		else
		{
			w += d;
		}
	}
	
	code("read", w);
  
	if (token != ")")
	{
		processError("',' or ')' expected; found " + token);
	}

	//code("read", x);
	nextToken();

	if (token != ";")
	{
		processError("';' expected; found " + token);
	}
}


void Compiler::writeStmt()     
{
	string x;
	if (token != "write")
	{
		processError("write expected; found " + token);
	}

	nextToken();

	if (token != "(")
	{
		processError("'(' expected after \"write\"");
	}

	nextToken();
	x = ids();

	if (token != ")")
	{
		processError("',' or ')' expected; found " + token);
	}

	code("write", x);
	nextToken();

	if (token != ";")
	{
	processError("';' expected");
	}
}

void Compiler::express()       
{
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
	{
		processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", non - keyword identifier or integer expected" + token);
	}

	term();

	if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">")
	{
		expresses();
	}
}
void Compiler::expresses()     
{
	string first, second;

	if (token != "=" && token != "<>" && token != "<=" && token != ">=" && token != "<" && token != ">")
	{
		processError("\"=\", \"<>\", \"<=\", \">=\", \"<\", or \">\" expected");  
	}

	pushOperator(token);
	nextToken();

	term();

	second = popOperand();
	first = popOperand();

	code(popOperator(), second, first);
	//cout << "$$" << second << " | " << first << "$$" << endl;

	if (token == "=" || token == "<>" || token == "<=" || token == ">=" || token == "<" || token == ">")
	{
		expresses();
	}
}
void Compiler::term()          
{
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
	{
		processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
	}

	factor();

	if (token == "-" || token == "+" || token == "or")
	{
	
		terms();
	}
}
void Compiler::terms()        
{
	string first, second;

	if (token != "+" && token != "-" && token != "or")
	{
		processError("\"+\", \"-\", or \"or\" expected");
	}

	pushOperator(token);
	nextToken();

	factor();
	second = popOperand();
	first = popOperand();

	code(popOperator(), second, first);
	//cout << "$$" << second << " | " << first << "$$" << endl;

 	if (token == "+" || token == "-" || token == "or")
	{
		terms();
	}
}
void Compiler::factor()      
{
	// FACTOR {'not','true','false','(','+','-',INTEGER,NON_KEY_ID}
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
	{
		processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", INTEGER, or NON_KEY_ID expected");
	}
	
	// PART 
	part();

	// FACTORS {'*','div','mod','and'}
	if (token == "*" || token == "div" || token == "mod" || token == "and")
	{
		factors();
	}

	//{'<>','=','<=','>=','<','>',')',';','-','+','or'}
	else if (isNonKeyId(token) || token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">" || token == ")" || token == ";" || token == "-" || token == "+" || token == "or" || token == "begin" || token == "do" || token == "then")
	{

	}
	
	else 
	{
		processError("expected '(', integer, or non_key_id" + token);
	}
}
void Compiler::factors()        
{
	string first, second;

	if (token != "*" && token != "div" && token != "mod" && token != "and")
	{
	  processError("\"*\", \"div\", \"mod\", or \"and\" expected");
	}

	pushOperator(token);
	nextToken();
	
	if (token != "not" && token != "(" && !isInteger(token) && !isNonKeyId(token) && token != "+" && token != "-" && token != "true" && token != "false")
	{
		processError("expected '(', integer, or non-keyword id " + token);
	}

	part();

	second = popOperand();		
	first = popOperand();
	
	code(popOperator(), second, first);
	//cout << "$$" << second << " | " << first << "$$" << endl;

	if (token == "*" || token == "div" || token == "mod" || token == "and")
	{
		factors();
	}
}
void Compiler::part()          
{
	string x = "";
	if (token == "not")
	{
		nextToken();
		
		if (token == "(") 
		{
			nextToken();
			
			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
			{
				processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
			}
			
			express();
			
			if (token != ")")
			{
				processError(") expected; found " + token);
			}
			
			nextToken();
			code("not", popOperand());
		}

		else if (isBoolean(token)) 
		{
			if (token == "true") 
			{
				pushOperand("false");
				nextToken();
			}
			else 
			{
				pushOperand("true");
				nextToken();
			}
		}

		else if (isNonKeyId(token)) 
		{
			code("not", token);
			nextToken();
		}
	}

	else if (token == "+")
	{
		nextToken();
		if (token == "(") 
		{
			nextToken();
			
			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
			{
				processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
			}
			
			express();
			
			if (token != ")")
			{
				processError("expected ')'; found " + token);
			}
			
			nextToken();
		}
		
		else if (isInteger(token) || isNonKeyId(token)) 
		{
			pushOperand(token);
			nextToken();
		}

		else 
		{
			processError("expected '(', integer, or non-keyword id; found " + token);
		}
	}

	else if (token == "-")
	{
		nextToken();
		
		if (token == "(") 
		{
			nextToken();
			
			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
			{
				processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
			}
			
			express();
			
			if (token != ")")
			{
				processError("expected ')'; found " + token);
			}
			
			nextToken();
			code("neg", popOperand());
		}
		
		else if (isInteger(token)) 
		{
			pushOperand("-" + token);
			nextToken();
		}
		
		else if (isNonKeyId(token)) 
		{
			code("neg", token);
			nextToken();
		}
	}

	else if (token == "(") 
	{
		nextToken();
		
		if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
		{
			processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
		}
		
		express();
		
		if (token != ")") 
		{
			processError(") expected; found " + token);
		}
		
		nextToken();
	}

	else if (isInteger(token) || isBoolean(token) || isNonKeyId(token)) 
	{
		pushOperand(token);
		nextToken();
	}

	else 
	{
		processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, boolean, or non - keyword identifier expected");
	}

}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Helper functions for the Pascallite lexicon
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
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
bool Compiler::isSpecialSymbol(char c) const
{
	vector<char> symb {'=', ':', ',', '.', '+', '-', ';', ':', '*', '<', '>', '(', ')'};
	
	for(uint i = 0; i < symb.size(); ++i)
	{
		if(c == symb[i])
			return true;
	}
	
	return false;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
bool Compiler::isBoolean(string s) const
{
	if(s == "true" || s == "false")
		return true;
	
	if(s == "boolean")
		return true;
	
	return false;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
bool Compiler::isLiteral(string s)	const
{
	if(isInteger(s) || isBoolean(s))
		return true;
	else if(s == "not" || s == "+" || s == "-")
		return true;
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Action routines
void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
	//cout << externalName;
	
	if (inType == PROG_NAME)
		return;
	
	string name, jic;
		name = "";
		jic = "";
		
		/*
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
		*/
		//cout << name << " " << jic << endl;
		for(uint i = 0; i < externalName.length(); ++i)
		{
			name = "";
			while(externalName[i] != ',' && i < externalName.length())
			{
				name += externalName[i];
				++i;
			}
			
			if(symbolTable.count(name) > 0)
				processError("multiple name definition");
		
				else if(isKeyword(name) && name != "true" && name != "false")
					processError("illegal use of keyword");
		
			else
			{
						
				/* if(isupper(name[0]))
					symbolTable.insert ({name, SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)}); */
			
				/* else
				{
					symbolTable.insert ({name, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)});
					//cout << " | " << name << endl;
				} */
				
				if (isupper(name[0]) || name == "true" || name == "false")
				{
					if (name == "true")
					{
						symbolTable.insert({name.substr(0, 15), SymbolTableEntry("TRUE", inType, inMode, inValue, inAlloc, inUnits)});
					}
					else if (name == "false")
					{
						symbolTable.insert({name.substr(0, 15), SymbolTableEntry("FALSE", inType, inMode, inValue, inAlloc, inUnits)});
					}
					else 
					{
						symbolTable.insert ({name.substr(0, 15), SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)});
					}
				}
				else
				{
					symbolTable.insert({name.substr(0, 15), SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)});
				}
			}
		}		
	
	/* if(symbolTable.count(name) > 0)
		processError("multiple name definition");
	
	else if(isKeyword(name) && name != "true" && name != "false")
		processError("illegal use of keyword");
	
	else
	{
					
		if(isupper(name[0]))
			symbolTable.insert ({name, SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)});
		
		else
		{
			symbolTable.insert ({name, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)});
			cout << " | " << name << endl;
		}
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
					cout << "$" << jic << endl
					symbolTable.insert ({jic, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)});
				}
			}
		} */
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
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
			//listingFile << endl << "name in question: " << name << endl;
			processError("reference to undefined constant");
			
		}
	}
			
	return dataType;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void Compiler::code(string op, string operand1, string operand2)
{
	if(op == "program")
		emitPrologue(operand1);
	
	else if(op == "end")
		emitEpilogue();
	else if(op == "write")
		emitWriteCode(operand1, "");
	else if(op == "read")
		emitReadCode(operand1, "");
	else if(op == "+")
		emitAdditionCode(operand1, operand2);
	else if(op == "-")
		emitSubtractionCode(operand1, operand2);
	else if(op == "neg")
		emitNegationCode(operand1, op);
	else if(op == "not")
		emitNotCode(operand1, op);
	else if(op == "*")
		emitMultiplicationCode(operand1, operand2);
	else if(op == "div")
		emitDivisionCode(operand1, operand2);
	else if(op == "mod")
		emitModuloCode(operand1, operand2);
	else if(op == "and")
		emitAndCode(operand1, operand2);
	else if(op == "or")
		emitOrCode(operand1, operand2);
	else if(op == "<")
		emitLessThanCode(operand1, operand2);
	else if(op == "<=")
		emitLessThanOrEqualToCode(operand1, operand2);
	else if(op == ">")
		emitGreaterThanCode(operand1, operand2);
	else if(op == ">=")
		emitGreaterThanOrEqualToCode(operand1, operand2);
	else if(op == "<>")
		emitInequalityCode(operand1, operand2);
	else if(op == "=")
		emitEqualityCode(operand1, operand2);
	else if(op == ":=")
		emitAssignCode(operand1, operand2);
	
	else
		processError("compiler error since function code should not be called illegal arguments");
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void Compiler::pushOperator(string op)
{
	operatorStk.push(op);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
string Compiler::popOperator()
{
	string t;
	
	if (!operatorStk.empty())
	{
		t = operatorStk.top();
		operatorStk.pop();
	}
	
	else
	{
		processError("compiler error; operator stack underflow");
	}
	
	return t;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void Compiler::pushOperand(string operand)
{
	if ((isInteger(operand) || isBoolean(operand)) && symbolTable.count(operand) == 0)
	{
		insert(operand, whichType(operand), CONSTANT, whichValue(operand), YES, 1);
	}
	//if name is a literal, also create a symbol table entry for it
	if (symbolTable.count(operand) == 0 && isLiteral(operand))
	{
		//symbolTable.insert(operand, whichType(operand), VARIABLE, whichValue(operand), YES, 1);
		symbolTable.insert ({operand, SymbolTableEntry(operand, whichType(operand), VARIABLE, whichValue(operand), YES, 1)});
	}
	operandStk.push(operand);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
string Compiler::popOperand()
{
	string t;
	
	if (!operandStk.empty())
	{
		t = operandStk.top();
		operandStk.pop();
	}
	
	else
	{
			processError("compiler error; operand stack underflow");
	}
	
	return t;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Emit Functions
void Compiler::emit(string label, string instruction, string operands, string comment)
{
	objectFile << left << setw(8) << label;
	objectFile << left << setw(8) << instruction;
	objectFile << left << setw(24) << operands << left << comment << endl;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitPrologue(string progName, string s)
{
	time_t result = time(nullptr);
	objectFile << "; JONATHAN MORGAN, CARLOS CARDENAS " << ctime(&result);
	objectFile << "%INCLUDE \"Along32.inc\"\n" << "%INCLUDE \"Macros_Along.inc\"\n" << endl;
	
	emit("SECTION", ".text");
	emit("global", "_start", "", "; program " + progName);
	
	objectFile << endl;
	
	emit("_start:");
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitEpilogue(string s1, string s2)
{
	emit("", "Exit", "{0}", "");
	objectFile << endl;
	emitStorage();
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

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
	
	objectFile << endl;
	emit("SECTION", ".bss", "", "");
	
	for(auto it = symbolTable.cbegin(); it != symbolTable.cend(); ++it)
	{	
		if(((*it).second.getAlloc() == YES) && ((*it).second.getMode() == VARIABLE))
			emit((*it).second.getInternalName(), "resd", "1", ("; "+(*it).first));
	}
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitReadCode(string operand, string)
{
	string name;
	unsigned int size = operand.size();
	for(unsigned int loopC = 0; loopC < size; ++loopC)
	{
		
		if(operand[loopC] != ',' && loopC < size)
		{
			name += operand[loopC];
			continue;
		}
		
		if(name != "")
		{
			if(symbolTable.count(name) == 0)
			{
				//cout << "here " << name;
				processError("reference to undefined symbol " + name);
			}
			
			if(symbolTable.at(name).getDataType() != INTEGER)
			{
				processError("can't read variables of this type");
			}
			
			if(symbolTable.at(name).getMode() != VARIABLE)
			{
				processError("attempting to read to a read-only location");
			}
			
			emit("", "call", "ReadInt", "; read int; value placed in eax");
			
			emit("", "mov", "[" + symbolTable.at(name).getInternalName() + "],eax", "; store eax at " + name);
			
			contentsOfAReg = symbolTable.at(name).getInternalName();
			
		}
		
		name = "";
	}
	
	if(name != "")
	{ 
		if(symbolTable.count(name) == 0)
		{
			//cout << "here " << name << endl;
			//cout << "$$" << name << "$$" << endl;
			processError("reference undefined symbol " + name);
			
		}
		
		if(symbolTable.at(name).getDataType() != INTEGER)
		{
			processError("can't read variables of this type");
		}
		
		if(symbolTable.at(name).getMode() != VARIABLE)
		{
			processError("attempting to read to a read-only location");
		}
		
		emit("", "call", "ReadInt", "; read int; value placed in eax");
		
		emit("", "mov", "[" + symbolTable.at(name).getInternalName() + "],eax", "; store eax at " + name);
		
		contentsOfAReg = symbolTable.at(name).getInternalName();
	}
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitWriteCode(string operand, string)
{
	string name;
	static bool definedStorage = false;
	unsigned int size = operand.size();
	
	for(unsigned int loopC = 0; loopC < size; ++loopC)
	{
		if(operand[loopC] != ',' && loopC < size)
		{
			name += operand[loopC];
			continue;
		}
		
		if(name != "")
		{
			if(symbolTable.count(name) == 0)
			{
				//cout << "here " << name << endl;
				processError("reference to undefined symbol " + name);
			}
			
			if(symbolTable.at(name).getInternalName() != contentsOfAReg)
			{
				emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "]", "; load " + name + " in eax");
				
				contentsOfAReg = symbolTable.at(name).getInternalName();
			}
			
			if(symbolTable.at(name).getDataType() == storeTypes::INTEGER)
			{
				emit("", "call", "WriteInt", "; write int in eax to standard out");
			}
			
			else
			{
				emit("", "cmp", "eax,0", "; compare to 0");
				
				string firstL = getLabel();
				
				emit("", "je", "." + firstL, "; jump if equal to print FALSE");
				
				emit("", "mov", "edx,TRUELIT","; load address of TRUE literal in edx");
				
				string secondL = getLabel();
				
				emit("", "jmp", "." + secondL, "; unconditionally jump to ." + secondL);
				
				emit("." + firstL + ":");
				
				emit("", "mov", "edx,FALSLIT", "; load address of FALSE literal in edx");
				
				emit("." + secondL + ":");
				
				emit("", "call", "WriteString", "; write string to standard out");
				
				if(definedStorage == false)
				{
					definedStorage = true;
					
					objectFile << endl;
					
					emit("SECTION", ".data");
					
					emit("TRUELIT", "db", "'TRUE',0", "; literal string TRUE");
					emit("FALSLIT", "db", "'FALSE',0", "; literal string FALSE");
					
					objectFile << endl;
					
				}
			}
			
										
			emit("", "call", "Crlf", "; write \\r\\n to standard out");
		}
		name = "";
	}
	
	if(name != "")
	{
		if(symbolTable.count(name) == 0)
		{
			//cout << "here " << name << endl;
			processError("reference to undefined symbol " + name);
		}
		
		if(symbolTable.at(name).getInternalName() != contentsOfAReg)
		{
			emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "]", "; load " + name + " in eax");
			contentsOfAReg = symbolTable.at(name).getInternalName();
		}
		
		if(symbolTable.at(name).getDataType() == storeTypes::INTEGER)
		{
			emit("", "call", "WriteInt", "; write int in eax to standard out");
		}
		
		else
		{
			emit("", "cmp", "eax,0", "; compare to 0");
			
			string firstL = getLabel();
			
			emit("", "je", "." + firstL, "; jump if equal to print FALSE");
			
			emit("", "mov", "edx,TRUELIT", "; load address of TRUE literal in edx");
			
			string secondL = getLabel();
			
			emit("", "jmp", "." + secondL, "; unconditionally jump to." + secondL);
			
			emit("." + firstL + ":");
			
			emit("", "mov", "edx,FALSLIT", "; load address of FALSE literal in edx");
			
			emit("." + secondL + ":");
			
			emit("", "call", "WriteString", "; write string to standard out");
			
			if(definedStorage == false)
			{
				definedStorage = true;
				
				objectFile << endl;
				
				emit("SECTION", ".data");
				
				emit("TRUELIT", "db", "'TRUE',0", "; literal string TRUE");
				
				emit("FALSLIT", "db", "'FALSE',0", "; literal string FALSE");
				
				objectFile << endl;
				
				emit("SECTION", ".text");
			}
		}
		
		emit("", "call", "Crlf", "; write \\r\\n to standard out");
	}
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitAssignCode(string operand1, string operand2)       
{
	if(symbolTable.count(operand1) == 0)
	{
		//cout << "here " << operand1 << endl;
		processError("reference to undefined symbol " + operand1);
	}
	
	else if(symbolTable.count(operand2) == 0)
	{
		//cout << "here " << operand2 << endl;
		processError("reference to undefined symbol " + operand2);
	}
	
	if(symbolTable.at(operand1).getDataType() !=symbolTable.at(operand2).getDataType())
	{
		//cout << "$$" << operand1 << " | " << operand2 << "$$" << endl;
		processError("incompatible types for operator ':='");
	}
	
	if(operand1 == operand2)
	{
		return;
	}
	//cout << "operand1: " << operand1 << " operand2: " << operand2 << " contentsOfAReg: " << contentsOfAReg << endl;
	
	if(contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "[" + symbolTable.at(operand1).getInternalName() + "],eax", "; " + operand1 + " = AReg");
		
		contentsOfAReg = symbolTable.at(operand1).getInternalName();
		
		 if(isTemporary(operand1))
		{
			freeTemp();
		} 
	}
	
	else if(contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
	
	
		emit("", "mov", "[" + symbolTable.at(operand2).getInternalName() + "],eax", "; " + operand2 + " = AReg");
	
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	
		if(isTemporary(operand1))
		{
			freeTemp();
		}
	}
	
	
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitAdditionCode(string operand1, string operand2)     
{
	
	if(symbolTable.count(operand1) == 0)
	{
		//cout << "here " << operand1 << endl;
		processError("reference to undefined symbol " + operand1);
	}
	
	else if(symbolTable.count(operand2) == 0)
	{
		//cout << "here " << operand2 << endl;
		processError("reference to undefined symbol " + operand2);
	}
	
	if(symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	{
		processError("binary '+' requires integer operands");
	}
	
	if(symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
	{
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		
		contentsOfAReg = "";
	}
	
	if(symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	
	if(symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg)
	{
	emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
	contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}
	
	if(contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("", "add", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " + " + operand1);
	}
	
	else
	{
		emit("", "add", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " + " + operand2);
	} 
	
	if(isTemporary(operand1))
	{
		freeTemp();
	}
	if(isTemporary(operand2))
	{
		freeTemp();
	}
	
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitSubtractionCode(string operand1, string operand2)    
{
	if(symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if(symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}
	
	if(symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	{
		processError("illegal type. binary '-' requires integer operands");
	}
	
	if(isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		
		contentsOfAReg = "";
	}
	
	if(!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}
	
	if(contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}
	
	if(contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("", "sub", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 +  " - " + operand1);
	}
	
	if(isTemporary(operand1))
	{
		freeTemp();
	}
	if(isTemporary(operand2))
	{
		freeTemp();
	}
	
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitMultiplicationCode(string operand1, string operand2) 
{
	if(symbolTable.count(operand1) == 0) 
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}
	
	if(symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	{
		processError("Illegal type");
	}
	
	if(isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov","[" + contentsOfAReg + "],eax","; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	
	if(!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}
	
	if(symbolTable.at(operand1).getInternalName() != contentsOfAReg && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov","eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}
	
	if(contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","imul","dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " * " + operand1);
	}
	
	else
	{
		emit("","imul","dword [" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " * " + operand2);
	}
	
	if(isTemporary(operand1))
	{
		freeTemp();
	}
	if(isTemporary(operand2))
	{
		freeTemp();
	}
	
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
	
void Compiler::emitDivisionCode(string operand1, string operand2)       
{
	if(symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}
	
	else if(symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}
	
	if(symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	{
		processError("binary 'div' requires integer operands");
	}
	
	if(isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov","[" + contentsOfAReg + "],eax", ";deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	
	if(isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}
	
	if(symbolTable.at(operand2).getInternalName() != contentsOfAReg)
	{
		emit("","mov","eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}
	
	emit("","cdq","","; sign extend dividend from eax to edx:eax");
	
	emit("","idiv","dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);
	
	if(isTemporary(operand1))
	{
		freeTemp();
	}
	if(isTemporary(operand2))
	{
		freeTemp();
	}
	
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void Compiler::emitModuloCode(string operand1, string operand2)        
{
	if(symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}
	
	else if(symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}
	
	if(symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	{
		processError("binary 'mod' requires integer operands");
	}
	
	if(isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov","[" + contentsOfAReg + "],eax", "; deassign AReg");
		
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		
		contentsOfAReg = "";
		
	}
	
	if(!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}
	
	if(symbolTable.at(operand2).getInternalName() != contentsOfAReg)
	{
		emit("", "mov","eax,[" + symbolTable.at(operand2).getInternalName() + "]","; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}
	
	emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");
	
	emit("","idiv","dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);
	
	emit("", "xchg", "eax,edx", "; exchange quotient and remainder");
	
	if(isTemporary(operand1))
	{
		freeTemp();
	}
	
	if(isTemporary(operand2))
	{
		freeTemp();
	}
	
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitNegationCode(string operand1, string)           
{
	if(symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	if(symbolTable.at(operand1).getDataType() != INTEGER)
	{
		processError("illegal type");
	}
	
	if(isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		emit("","mov","[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	
	if(!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		contentsOfAReg = "";
	}
	
	if(contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		emit("","mov","eax,[" + symbolTable.at(operand1).getInternalName() + "]","; AReg = " + operand1);
		contentsOfAReg = symbolTable.at(operand1).getInternalName();
	}
	
	emit("","neg","eax","; AReg = -AReg");
	
	if(isTemporary(operand1))
	{
		freeTemp();
	}
	
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg);
	
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitNotCode(string operand1, string)               
{
	if(symbolTable.count(operand1) == 0) 
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	if(symbolTable.at(operand1).getDataType() != BOOLEAN)
	{
		processError("illegal type");
	}
	
	if(isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		emit("","mov","[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	
	if(!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		contentsOfAReg = "";
	}
	
	if(contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		emit("","mov","eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
		contentsOfAReg = symbolTable.at(operand1).getInternalName();
	}
	
	emit("","not","eax","; AReg = !AReg");
	
	if(isTemporary(operand1))
	{
		freeTemp();
	}
	
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	
	pushOperand(contentsOfAReg);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitAndCode(string operand1, string operand2)         
{
	if(symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if(symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}
	
	if(symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
	{
		processError("binary 'and' requires boolean operands");
	}
	
	if(isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov","[" + contentsOfAReg + "],eax", "; deassign AReg");
		
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		
		contentsOfAReg = "";
	}
	
	if(!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}
	
	if(contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg !=  symbolTable.at(operand2).getInternalName())
	{
		emit("","mov","eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}
	
	if(contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","and","eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " and " + operand1);
	}
	
	if(isTemporary(operand1))
	{
		freeTemp();
	}
	
	if(isTemporary(operand2))
	{
		freeTemp();
	}
	
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	
	pushOperand(contentsOfAReg);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitOrCode(string operand1, string operand2)           
{
	if(symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if(symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}
	
	if(symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
	{
		processError("illegal type");
	}
	
	if(isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov","[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	
	if(!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}
	
	if(contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov","eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}
	
	if(contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","or","eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " or " + operand1);
	}
	
	else if(contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","or", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " or " + operand2);
	}
	
	if(isTemporary(operand1))
	{
		freeTemp();
	}
	
	if(isTemporary(operand2))
	{ 
		freeTemp();
	}
	
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	
	pushOperand(contentsOfAReg);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void Compiler::emitEqualityCode(string operand1, string operand2)      
{
	if(symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if(symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}
	
	if(symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}
	
	if(isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
		{
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}

	// if neither operand is in the A register then
	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	//emit code to perform a register-memory compare
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}

	//if equal emit code to jump to next available 
	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","je", "." + newLabel, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");
	}
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","je", "." + newLabel, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");
	}

	//emit FALSE into the A register
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
	}

	string secondLabel = getLabel();
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
	}

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
	{
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

	pushOperand(contentsOfAReg);
}


void Compiler::emitInequalityCode(string operand1, string operand2)    
{
	if (symbolTable.count(operand1) == 0)
	{
	    processError("reference to undefined symbol " + operand1);
	}
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}
	
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types for operator '<>'");
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName()) 
	{
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

		symbolTable.at(contentsOfAReg).setAlloc(YES);

		contentsOfAReg = "";
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName()) 
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}

	string label_1 = getLabel(), label_2 = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("", "jne", "." + label_1, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");
	}
	else
	{
		emit("", "jne", "." + label_1, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");
	}

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0) 
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
	}
	
	emit("", "jmp", "." + label_2, "; unconditionally jump");

	emit("." + label_1 + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0) 
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
	}

	emit("." + label_2 + ":");

	if (isTemporary(operand1))
	{
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

	pushOperand(contentsOfAReg);
}
void Compiler::emitLessThanCode(string operand1, string operand2)       
{
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}

	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","jl", "." + newLabel, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","jl", "." + newLabel, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
	}

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	}

	string secondLabel = getLabel();
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	}

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
	{
		freeTemp();
	}
	
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

	pushOperand(contentsOfAReg);
}
void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2) 
{
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}

	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","jle", "." + newLabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","jle", "." + newLabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
	}

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	}

	string secondLabel = getLabel();
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	}

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
	{
		freeTemp();
	}
	
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

	pushOperand(contentsOfAReg);
}
void Compiler::emitGreaterThanCode(string operand1, string operand2)    
{
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}

	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","jg", "." + newLabel, "; if " + operand2 + " > " + operand1 + " then jump to set eax to TRUE");
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","jg", "." + newLabel, "; if " + operand1 + " > " + operand2 + " then jump to set eax to TRUE");
	}

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	}

	string secondLabel = getLabel();
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	}

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
	{
		freeTemp();
	}
	
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

	pushOperand(contentsOfAReg);
}
void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2) 
{
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}

	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","jge", "." + newLabel, "; if " + operand2 + " >= " + operand1 + " then jump to set eax to TRUE");
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","jge", "." + newLabel, "; if " + operand2 + " >= " + operand1 + " then jump to set eax to TRUE");
	}

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
	}

	string secondLabel = getLabel();
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
	}

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
	{
		freeTemp();
	}
	
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

	pushOperand(contentsOfAReg);
}

// Other routines
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

void Compiler::processError(string err)
{
	listingFile << '\n' << "Error: Line " << lineNo << ": " << err << "\n" << endl;
	
	listingFile <<"COMPILATION TERMINATED      " << "1 ERRORS ENCOUNTERED" << endl;
	
	exit(1);
}

void Compiler::freeTemp()
{
	currentTempNo--;
	if (currentTempNo < -1)
	{
		processError("compiler error, currentTempNo should be >= –1");
	}
}

string Compiler::getTemp()
{
	string temp;
	currentTempNo++;

	temp = "T" + to_string(currentTempNo);

	if (currentTempNo > maxTempNo)
	{
		insert(temp, UNKNOWN, VARIABLE, "1", NO, 1);
		symbolTable.at(temp).setInternalName(temp);
		maxTempNo++;
	}
	
	return temp;
}
string Compiler::getLabel()
{
	string label;
	static int count = 0;
	
	label = "L" + to_string(count);
	
	count++;

	return label;
}

bool Compiler::isTemporary(string s) const 
{
	if (s[0] == 'T') 
	{
		return true;
	}
	return false;
}

string Compiler::nextToken()
{
	token = "";	
	while(token == "")
	{
		if (ch == '{')
		{
			while (nextChar() != END_OF_FILE && ch != '}')
			{
				
			}
			
			if (ch == END_OF_FILE)
			{
				processError("unexpected end of file: '}' expected");
			}
			
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

			if (token == ":" && ch == '=') 
			{
				token += ch;
				nextChar();
			}
			
			if ((token == "<" && ch == '=') || (token == "<" && ch == '>') || (token == ">" && ch == '=')) 
			{
				token += ch;
				nextChar();
			}
		}
		
		else if (islower(ch))
		{
			token = ch;

			while((nextChar() == '_' || islower(ch) || isupper(ch) || isdigit(ch)) && ch != END_OF_FILE)
			{
				token = token + ch;
			}

			if (ch == END_OF_FILE)
			{
				processError("unexpected end of file");
			}
		}
		
		else if (isdigit(ch))
		{
			token = ch;
			
			while (nextChar() != END_OF_FILE && isdigit(ch))
			{
				token += ch;
			}

			if (ch == END_OF_FILE)
			{
				processError("unexpected end of file");
			}
		}

		else if (ch == END_OF_FILE)
		{
			token = ch;
		}
		
		else
		{
			//cout << ch;
			
			processError("illegal symbol");
		}
	}
	
	token = token.substr(0,15);

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