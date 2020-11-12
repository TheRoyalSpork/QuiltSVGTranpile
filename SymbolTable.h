#include "AST.h"

class SymbolTable {
	map<QuiltNodeId,QuiltNode *> symbols;
	public:
	bool exists(QuiltNodeId *id) { return symbols.find(*id)!=symbols.end(); }
	void add(QuiltNode *node,QuiltNode *ast) {
		//cout << "In Add " << endl; 
		//cout << node << endl;
		symbols[(QuiltNodeId)(*node)]=ast; 
	}
	friend ostream & operator <<(ostream &out,SymbolTable &st) {
      out << "Symbol Table " <<endl;
	  for (auto& [key, value]: st.symbols)
	    out << key << "->" << value << endl;
      return out<<endl;
	}
	~SymbolTable(){
	  for (auto& [key, value]: symbols)
	    value->cleanUp();
	}
};

extern SymbolTable symbolTable;
