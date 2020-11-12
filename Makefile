CPP=g++
CPPFLAGS=-Wall -g -std=gnu++17
MAINSRC=ExpressionParse.cpp
OBJS=Testing.o Tokenizer.o SymbolTable.o

ExpressionParse: $(OBJS) $(MAINSRC) Testing.h Syntax.h AST.h SVGArt.h SymbolTable.h
	$(CPP) -o $@ $(OBJS) $(MAINSRC) $(CPPFLAGS)
	
.cpp.o: 
	$(CPP) -c -o $@ $< $(CPPFLAGS)
