#include <iostream>
#include <regex>
#include <sstream>
#include <fstream>
#include <vector>

#include "Testing.h"
#include "Syntax.h"
#include "SVGArt.h"
#include "Tokenizer.h"
#include "AST.h"
#include "SymbolTable.h"

using namespace std;

bool quiltExpression(Tokenizer &tokens,QuiltNode *ast);

bool sewExpression(Tokenizer &tokens,QuiltNode *ast) {
        tokens.next();
		Token t=tokens.next();
		if (t!=OPENPAREN) throw SyntaxError("Expected ( after sew",tokens.location());
		QuiltNode *left=new QuiltNode();
		if (!quiltExpression(tokens,left)) throw SyntaxError("Expected quilt in sew",tokens.location());
		t=tokens.next();
		if (t!=COMMA) throw SyntaxError("Expected , after first argument to sew ",tokens.location());
		QuiltNode *right=new QuiltNode();		
		if (!quiltExpression(tokens,right)) throw SyntaxError("Expected quilt in sew",tokens.location());
		t=tokens.next();
		if (t!=CLOSEPAREN) throw SyntaxError("Expected ) after turn ",tokens.location());
		QuiltNode *treePart=ast->setOrAdd(SEW);
		treePart->addChild(left);
		treePart->addChild(right);
		return true;
}

bool functionDeclaration(Tokenizer &tokens,QuiltNode *ast) {
	Token t=tokens.peek();
	if (t==FUN) {
		tokens.next();  // Remove fun from string
		t=tokens.next();
		if (t==NAME) {
		   QuiltNode *funDecl=new QuiltNode(FUN);
		   funDecl->setName(tokens.name);
		   t=tokens.next();
		   if (t==OPENPAREN) {
			   t=tokens.next();
			   if (t==NAME) {
				  if (t!=NAME) throw SyntaxError("Expected parameter formal argument name in function decl ",tokens.location());
				  QuiltNode *argDecl=new QuiltNode(PARAMETER);
				  argDecl->setName(tokens.name);
				  funDecl->addChild(argDecl);
				  t=tokens.next();
				  while (t==COMMA) {
					  t=tokens.next();  // grab the variable name
				      if (t!=NAME) throw SyntaxError("Expected parameter another formal argument name in function decl ",tokens.location());
					  QuiltNode *argDecl=new QuiltNode(PARAMETER);
				      argDecl->setName(tokens.name);
				      funDecl->addChild(argDecl);
					  t=tokens.next(); // should be , or )
				  }
				  if (t==CLOSEPAREN) {
					t=tokens.next();
					if (t!=EQUALS) throw SyntaxError("Expected = in function declaration",tokens.location());
					QuiltNode *expr=new QuiltNode(EXPRESSION);
					if (quiltExpression(tokens,expr)) {
						funDecl->addChild(expr);
						//cout << "Should segfault because it does later " << endl;
						//cout << funDecl << endl;
						ast->addChild(funDecl);
						if (!symbolTable.exists(funDecl)) 
				          symbolTable.add(funDecl,funDecl);
				        else 
				          throw SemanticError("Symbol already defined",tokens.location());
						return true;
					}
					throw SyntaxError("Expected a sub expression in function declaration",tokens.location());
				  }
				  throw SyntaxError("Expected a ) or , in function declaration",tokens.location());
		     }
		     throw SyntaxError("Expected a parameter name",tokens.location());
		   }
		   throw SyntaxError("Expected ( in function declaration ",tokens.location());
		}
	    throw SyntaxError("Expected a function name ",tokens.location());
	}
	return false;
} 

/* Parsing the syntax of a function use */
bool functionUse(Tokenizer &tokens,QuiltNode *ast) {
	Tokenizer temp=tokens;
	Token t=temp.next();
	Token t2=temp.peek();
	if (t==NAME && t2==OPENPAREN) {
        t=tokens.next();
		QuiltNode *funUse=new QuiltNode(FUNCALL);
		funUse->setName(tokens.name);
		t=tokens.next(); // Remove the OpenParen token
        tokens.check();
        QuiltNode *temp=new QuiltNode();
        if (quiltExpression(tokens,temp)) {
		    QuiltNode *argActual=new QuiltNode(PARAMETER);
			argActual->addChild(temp);
			funUse->addChild(argActual);
			t=tokens.next();
			while (t==COMMA) {
                  QuiltNode *temp=new QuiltNode();
				  if (!quiltExpression(tokens,temp)) 
				    throw SyntaxError("Expected parameter name in function use",tokens.location());
				  QuiltNode *argActual=new QuiltNode(PARAMETER);
				  argActual->addChild(temp);
				  funUse->addChild(argActual);
				  t=tokens.next(); // should be , or )
			  }
			  if (t==CLOSEPAREN) {
				    ast->addChild(funUse);
				    if (symbolTable.exists(funUse)) {
						// Do the work of calling the function for interpreter
						// Ready for code generation for a compiler
						return true;
					} 
					else
					  throw SemanticError("Function called but not declared",tokens.location());
				    // Function Call/Use interact with symbol table
					
			  }  throw SyntaxError("Expected ) or , in function use ",tokens.location());
			} 
			else throw SyntaxError("Expected Quilt Expression parameter in function use",tokens.location());
	} 
	return false;
}

// From Matt Behnke 
bool valDeclaration(Tokenizer &tokens,QuiltNode *ast) {
	// Finished
	// Examples:
	// 		val aa = A
	// 		val bb = turn(B)
	// 		val thing = mysew(turn(A),sew(A,B))
	Token t=tokens.peek();
	if (t == VAL) {
		tokens.next();
		t = tokens.next();
		if(t == NAME) {
			QuiltNode *valDecl=new QuiltNode(VAL);
			valDecl->setName(tokens.name);
			t=tokens.next();
			if(t == EQUALS) {
				QuiltNode *expr=new QuiltNode(EXPRESSION);
				if (quiltExpression(tokens,expr)) {
						valDecl->addChild(expr);
						ast->addChild(valDecl);
						return true;
				} 
				throw SyntaxError("Expected a sub expression in function declaration",tokens.location());
			} throw SyntaxError("Expected '=' in val decleration", tokens.location());
		} throw SyntaxError("Expected a name for value", tokens.location());
	} throw SyntaxError("Expected a val name", tokens.location());

}

// From Matt Behnke
bool letExpression(Tokenizer &tokens,QuiltNode *ast) {
	// Example to parse
	// let fun unturn(x) = turn( turn( turn(x) ) )
	//      fun pile (x,y) = unturn( sew( turn(y), turn(x) ) )
	//      val aa = pile(a, trun( turn(a) ) )
	//      val bb = pile( unturn(b), turn(b) )
	//      val p = sew(bb, aa)
	//      val q = sew(aa, bb)
	// in
	//       pile(p,q)
	// end
	Token t=tokens.peek();
	if (t == LET) {
		t=tokens.next();
		if(t == IN) throw SyntaxError("Expected either val or fun expression in let statement", tokens.location());
		QuiltNode *letDecl=new QuiltNode(LET);
		while(t != IN) {
			// Add as many val or fun declerations as possible
			QuiltNode *expr=new QuiltNode(EXPRESSION);
			if (quiltExpression(tokens,expr)) {
						letDecl->addChild(expr);
						ast->addChild(letDecl);
			} else {
				throw SyntaxError("Expected expression in let statement", tokens.location());
			} 
			t = tokens.peek();
		}
		if(t != IN) throw SyntaxError("Expected 'in' after expressions in let statement", tokens.location());
		t = tokens.next();
		cout << t << endl;
		t = tokens.next();
		if(t == NAME) {
			t = tokens.next();
			if(t == OPENPAREN) {
				t=tokens.next();
			    if (t==NAME) {
				  if (t!=NAME) throw SyntaxError("Expected parameter formal argument name in function decl ",tokens.location());
				  QuiltNode *argDecl=new QuiltNode(PARAMETER);
				  argDecl->setName(tokens.name);
				  letDecl->addChild(argDecl);
				  t=tokens.next();
				  while (t==COMMA) {
					  t=tokens.next();  // grab the variable name
				      if (t!=NAME) throw SyntaxError("Expected parameter another formal argument name in function decl ",tokens.location());
					  QuiltNode *argDecl=new QuiltNode(PARAMETER);
				      argDecl->setName(tokens.name);
				      letDecl->addChild(argDecl);
					  t=tokens.next(); // should be , or )
				  }
				  if (t==CLOSEPAREN) {
					  t = tokens.next();
						if(t == END) {
							return true;
						} throw SyntaxError("Expected 'end' in let statement", tokens.location());
				  } throw SyntaxError("Expected closing parenthesis", tokens.location());
				} throw SyntaxError("Expetced '(' after name", tokens.location());
			} throw SyntaxError("Expected Open Parenthsis", tokens.location());
		} throw SyntaxError("Expected funciton name after in statement", tokens.location());
	
	}throw SyntaxError("Expected keyword 'let'", tokens.location());
	return false;
}

bool quiltExpression(Tokenizer &tokens,QuiltNode *ast) {
	Token t=tokens.peek();

	Tokenizer temp=tokens;
    temp.next();
	Token t2=temp.peek();

	if (t==A)  {
		tokens.next();
		ast->setOrAdd(A);
		return true;
	}
	else if (t==B)  {
		tokens.next();
		ast->setOrAdd(B); 
		return true;
	}
	else if (t==NAME) {  // Variable use  Some Symbol Table Interaction here
		t=tokens.next();
		ast->setOrAdd(NAME);
		ast->setName(tokens.name);
		return true;
	}
	else if (t==TURN) {
		tokens.next(); 
		t=tokens.next();
		if (t!=OPENPAREN) throw SyntaxError("Expected ( after turn",tokens.location());
		QuiltNode *unary=new QuiltNode();
		if (!quiltExpression(tokens,unary)) throw SyntaxError("Expected quilt in turn",tokens.location());
		t=tokens.next();
		if (t!=CLOSEPAREN) throw SyntaxError("Expected ) after turn ",tokens.location());
		QuiltNode *treePart=ast->setOrAdd(TURN);
		treePart->addChild(unary);
		return true;
	}
	else if (t==SEW) {
		return sewExpression(tokens,ast);
	} else if (t==FUN) {
		return functionDeclaration(tokens,ast);
	} else if (t==LET) {
		return letExpression(tokens,ast);
	} else if (t==VAL) {
		return valDeclaration(tokens,ast);
	} else if (t==NAME && t2==OPENPAREN) {
		return functionUse(tokens,ast);
	}
	return false;
}

SVGArt *quiltExpressionHelper(string text,string filename,int linenumber)  {
	Tokenizer tokens(text,filename,linenumber);
	QuiltNode *root=new QuiltNode(EXPRESSION);
	quiltExpression(tokens,root);
	cout << "Expression tree is " << endl;
	cout << root << endl;
	SVGArt *output=root->eval();
	//if (output==NULL) cerr << "No SVG generated" << endl;
	// if (root!=NULL) cout << text << endl << root << endl;
	delete root;
	return output;
}

class ParseTest1:public Test {
	public:
	ParseTest1():Test("Simple working expression"){}
	bool checker() {
	  try {
	    if (NULL==quiltExpressionHelper("sew(turn(A),B)","ParseTest1",1)) return false;
	    cout << symbolTable << endl;
	    cout << "Example 1 pass" << endl;
	    if (NULL==quiltExpressionHelper("turn(sew(A,B))","ParseTest1",2)) return false;
	    cout <<  symbolTable << endl;
	    cout << "Example 2 pass" << endl;
	    if (NULL==quiltExpressionHelper("sew(turn(A),turn(B))","ParseTest1",3)) return false;
	    cout << symbolTable << endl;
	    cout << "Example 3 pass" << endl;
	    if (NULL==quiltExpressionHelper("turn(sew(sew(B,A),sew(A,B)))","ParseTest1",4)) return false;
	    cout <<  symbolTable << endl;
	    cout << "Example 4 pass" << endl;
	  return true; 
      } catch(SyntaxError e)  {
		  cerr << e << endl;
		  return false;
	  } 
	}
};

class ParseTest2:public Test {
	public:
	ParseTest2():Test("Function Declaration"){}
	bool checker() {
	  bool failed=false;
	  try {
	    if (NULL==quiltExpressionHelper("fun oneturn(x)=turn(x)","ParseTest2a",1))  {
		  cout <<  symbolTable << endl;
		  cerr << "Failed " << getName() << endl;
		  failed=true;
	    }
	    if (NULL==quiltExpressionHelper("fun mysew(x,y)=sew(x,y)","ParseTest2b",1)) {
		  cout <<  symbolTable << endl;
	      cerr << "Failed " << getName() << endl;
	      failed=true;
	    }
	    if (NULL==quiltExpressionHelper("fun quad(a,b,c,d)=sew(sew(a,b),sew(c,turn(d)))","ParseTest2b",1)) {
		  cout <<  symbolTable << endl;
	      cerr << "Failed " << getName() << endl;
	      failed=true;
	    }	  
	    return !failed; 
      } catch(SyntaxError e)  {
		  cerr << e << endl;
		  return false;
	  } catch(SemanticError e)  {
		  cerr << e << endl;
		  return false;
	  } 	
  }
};

class ParseTest3:public Test {
	public:
	ParseTest3():Test("Function Use of Function Call"){}
	bool checker() {
	  bool failed=false;
	  try {
	    if (NULL==quiltExpressionHelper("oneturn(a)","ParseTest3a",1)) {
 		    cout <<  symbolTable << endl;
			cerr << "Failed "<< getName() <<endl;
			failed=true;
		}
	    if (NULL==quiltExpressionHelper("mysew(a,b)","ParseTest3b",1)) {
		    cout <<  symbolTable << endl;
			cerr << "Failed "<< getName() <<endl;
			failed=true;
		}
	    if (NULL==quiltExpressionHelper("mysew(turn(A),sew(A,B))","ParseTest3b",1)) {
		    cout <<  symbolTable << endl;
			cerr << "Failed "<< getName() <<endl;
			failed=true;
		}
	    return !failed; 
      } catch(SyntaxError e)  {
		  cerr << e << endl;
		  return false;
	  } catch(SemanticError e)  {
		  cerr << e << endl;
		  return false;
	  } 
	}
};

void ParserTester() {
	tests.push_back(new ParseTest1());
	tests.push_back(new ParseTest2());
	tests.push_back(new ParseTest3());	
}

void Tests() {
//  TokenTester();
  ParserTester();
  runTests(); 
/*		LitA a;
		LitB b;
		b.turn();
		b.turn();
		b.turn();
		a.sew(b);
		a.turn();
		LitB c;
		LitB d;
		c.turn();
		c.turn();
		d.sew(c);
		d.turn();
		a.sew(d);
		cout << &a ;*/
}

int main(int argc,char **argv) {/*
  if (argc!=3) {
	  cerr << "ExpressParse.exe <input file> <output file>\n" << endl;
	  return -1;
  } else {
	  ifstream in;
	  in.open(argv[1]);
	  ofstream out;
	  out.open(argv[2]);
	  int lineNumber=1;
	  try {
	    while (!in.eof()) {
		  char buffer[1024];
		  in.getline(buffer,1024);
		  SVGArt *quilt=quiltExpressionHelper(buffer,string(argv[1]),lineNumber);
		  if (quilt!=NULL) 
		    out << quilt;
		  lineNumber++;
	    } 
      } catch(Error e) {
		  cerr << e ;
		  return -1;
	  }
	  in.close();
	  out.close();
  } */
  Tests();
  return 0;
}
