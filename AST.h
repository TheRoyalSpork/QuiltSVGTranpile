#pragma once

#include <map>

#include "Tokenizer.h"
#include "SVGArt.h"

using namespace std;

class QuiltNodeId{
	protected:
	Token operation;
	string name;
	public:
	QuiltNodeId(Token newOperation=EMPTYTOKEN) {
		operation=newOperation;
		name="";
	}
    bool operator <(const QuiltNodeId &other ) const {
		if (operation!=other.operation) return operation < other.operation;
		return name<other.name; 
	}
	friend ostream & operator <<(ostream &out,QuiltNodeId qn) {
		return out << "Token "<< (int)qn.operation << " ["<< qn.name <<"]"; 
	}
};

class QuiltNode:public QuiltNodeId {
	vector<QuiltNode *> children;
	public:
	QuiltNode(Token newOperation=EMPTYTOKEN):QuiltNodeId(newOperation) {
	}
	void setName(string newName) {
		name=newName;
	}
    void addChild(QuiltNode *newChild) {
		children.push_back(newChild);
	}
	QuiltNode *setOrAdd(Token t) {
	  if (operation==EMPTYTOKEN) {
		  operation=t;
		  return this;
	  }
	  else {
		  QuiltNode *treePart=new QuiltNode(t);
		  this->addChild(treePart);
		  return treePart;
	  } 
	}
	friend ostream &operator <<(ostream &out, QuiltNode *n) {
		switch(n->operation) {
			case A: out << "Lit A" ; break; 
			case B: out << "Lit B" ; break; 
			case SEW: out << "SEW  2 Children" ; break; 
			case TURN: out << "TURN a Child" ; break; 
			case EXPRESSION: out << "Expression "; break;
			case FUN: out << "FUN Declaration" ; break; 
			case NAME: out << "NAME " ; break; 
			case PARAMETER: out << "PARAMETER " ; break; 
			case FUNCALL: out << "FUN Call " ; break; 
			default: out << "Empty" ; break; 
		}
		out << " Number of children " << n->children.size()<< " Name:"<< n->name << endl;
		for(unsigned i=0;i<n->children.size();i++) 
		  out << n->children[i] << endl; 
		return out;
	}
	void cleanUp() {
		for(unsigned i=0;i<children.size();i++) 
           delete children[i];
	}
	SVGArt *eval() {
		// Memory leaks abound
		if (operation==A) return new LitA();
		else if (operation==B) return new LitB();
		else if (operation==TURN) {
			SVGArt *childArt=children[0]->eval();
			childArt->turn();
			return childArt;
 		} 
		else if (operation==SEW) {
			SVGArt *childArt1=children[0]->eval();
			SVGArt *childArt2=children[1]->eval();
			childArt1->sew(*childArt2);
			return childArt1;
		}
		else if (operation==EXPRESSION) {
		    if (children.size()==1) 
			  return children[0]->eval();
			else return NULL;
		}
		return NULL;
	}
};


