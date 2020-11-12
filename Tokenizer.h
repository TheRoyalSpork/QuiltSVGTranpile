#pragma once

#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

#include "Testing.h"

using namespace std;

enum Token {EMPTYTOKEN,A,B,TURN,SEW,OPENPAREN,CLOSEPAREN,COMMA,FUN,EQUALS
	  ,NAME,LET,IN,END,VAL,EXPRESSION,PARAMETER,FUNCALL};
	  
/*bool operator <(const Token &a,const Token &b) {
	return (int)a<(int)b;
}*/

class Tokenizer {
	private:
	  string s;
	  smatch sm;
	  int originalLength;
	  int lineNumber;
	  string filename;
	  int charNumber;
	public:
	string name;
    string location() {
		stringstream ss;
		ss << filename <<":"<<lineNumber << ":"<< charNumber<<":";
		return ss.str();
	}
	Tokenizer(string newString,string newFilename,int newLineNumber=0) {
	   lineNumber=newLineNumber;
	   filename=newFilename;
       s=newString;
        
	}
	// 
	Token peek() {
		Token t=EMPTYTOKEN;
		name="";
		regex keywordExpr("^(A|B|turn|sew|fun|let|in|end|val|=|\\(|\\)|,|[a-zA-Z]+)");
		regex nameExpr("^[a-zA-Z]+");
		if (regex_search(s,sm,keywordExpr) ) {
			if (sm[0]=="A") t=A;
			else if (sm[0]=="B") t=B;
			else if (sm[0]=="turn") t=TURN;
			else if (sm[0]=="sew") t=SEW;
			else if (sm[0]=="fun") t=FUN;
			else if (sm[0]=="let") t=LET;
			else if (sm[0]=="in") t=IN;
			else if (sm[0]=="end") t=END;
			else if (sm[0]=="val") t=VAL;
			else if (sm[0]=="=") t=EQUALS;
			else if (sm[0]=="(") t=OPENPAREN;
			else if (sm[0]==")") t=CLOSEPAREN;
			else if (sm[0]==",") t=COMMA;
			else if (regex_search(s,sm,nameExpr)) {
				t=NAME;
				name=sm[0];
			}
		}		
		return t;		
	}
	void pop() {
		// Remove next token
		s=sm.suffix().str();
		charNumber=originalLength-s.size();
		if (s[0]==' ') s=s.substr(1);
    }
	Token next() {
		//check();
		Token t=peek();
		pop();
		return t;
	}
	void check() {
		cerr << "Tokenizer String is :" <<s << ":" <<endl;
	}
};

class TTest1:public Test{
	public:
	TTest1():Test("Check Each Symbol"){
	}
	bool checker() {
	  Tokenizer tokenizer("A B turn sew ( ) ,","TTest1",1);
	  if (tokenizer.next()!=A) return false;
	  if (tokenizer.next()!=B) return false;
	  if (tokenizer.next()!=TURN) return false;
	  if (tokenizer.next()!=SEW) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false;
	  if (tokenizer.next()!=COMMA) return false;
   	  return true;
	} 
};

class TTest2:public Test{
	public:
	TTest2():Test("Actual Statement"){}
    bool checker() {
	  Tokenizer tokenizer("sew(turn(A),B)","TTest1",1);
	  if (tokenizer.next()!=SEW) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=TURN) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=A) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false;
	  if (tokenizer.next()!=COMMA) return false;
	  if (tokenizer.next()!=B) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false;
   	  return true;
  }
};

class TTest3:public Test{
	public:
	TTest3():Test("Function Statement"){}
    bool checker() {
	  Tokenizer tokenizer("fun oneturn(x)=turn(x)","TTest3",1);
	  if (tokenizer.next()!=FUN) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false;
	  if (tokenizer.next()!=EQUALS) return false;
	  if (tokenizer.next()!=TURN) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false;
   	  return true;
  }
};

class TTest4:public Test{
	public:
	TTest4():Test("Let Example"){}
    bool checker() {
	  Tokenizer tokenizer("let fun oneturn(x)=turn(x)"
	                      " fun pile(x,y)=sew(x,oneturn(x))"
	                      " in pile(oneturn(B),oneturn(A))"
	                      " end","TTest4",1);
	 /* fix test case for let example */
	  if (tokenizer.next()!=LET) return false; // Line 1	  
	  if (tokenizer.next()!=FUN) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false;
	  if (tokenizer.next()!=EQUALS) return false;
	  if (tokenizer.next()!=TURN) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false; 
      cout << "Line 1 pass" << endl;
	  if (tokenizer.next()!=FUN) return false; // Line 2
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=COMMA) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false;
	  if (tokenizer.next()!=EQUALS) return false;
	  if (tokenizer.next()!=SEW) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=COMMA) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false; 
      cout << "Line 2 pass" << endl;	  
	  if (tokenizer.next()!=IN) return false; // Line 3
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=B) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false;
	  if (tokenizer.next()!=COMMA) return false;
	  if (tokenizer.next()!=NAME) return false;
	  if (tokenizer.next()!=OPENPAREN) return false;
	  if (tokenizer.next()!=A) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false;
	  if (tokenizer.next()!=CLOSEPAREN) return false;
      cout << "Line 3 pass" << endl;
	  if (tokenizer.next()!=END) return false; // Line 4
	  cout << "Line 4 pass" << endl;

   	  return true;
  }
};

void TokenTester();
