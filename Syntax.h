#pragma once
#include <string>
#include <ostream>
using namespace std;

class Error{
	string location; // A file for the exception
	string explanation; // An explanation of the exception
	public:
	Error(string newExplanation,string newLocation) {
		explanation=newExplanation;
		location=newLocation;
	}
	friend ostream &operator <<(ostream &out,const Error &e) {
		return out<< e.location << " Exception "<< e.explanation 
		  << endl;
	}
};

class SyntaxError:Error {
	public:
	SyntaxError(string newExplanation,string newLocation)
	   :Error(newExplanation,newLocation) {	}
    friend ostream &operator <<(ostream &out,const SyntaxError &e) {
		return out << (Error)e << " Syntax Error";
	}	
};

class SemanticError:Error {
	public:
	SemanticError(string newExplanation,string newLocation)
	  :Error(newExplanation,newLocation) {	}
    friend ostream &operator <<(ostream &out,const SemanticError &e) {
		return out << (Error)e << " Semantic Error";
	}	
};
