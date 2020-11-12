#pragma once

class SVGArt {
	protected:
	string picture;
	int w,h;
	public:
	friend ostream &operator << (ostream &out, SVGArt *a) {
		  out 
		  <<"<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n"
            "<svg xmlns:svg='http://www.w3.org/2000/svg' xmlns='http://www.w3.org/2000/svg' "
            "width='"<<a->w<<"mm' height='"<<a->h<<"mm' viewBox='0 0 "<<a->w<<" "<<a->h<<"' version='1.1' >\n"
            "<g transform='translate("<< (a->w/2)<<" "<<(a->h/2)<<")'>\n"
		  << a->picture 
		  << "</g></svg>\n" 
		  << endl;
		  return out;
	}
	void turn(){
		picture="<g transform='rotate(90)'>"+picture+"</g>";
		int t=w;
		w=h;
		h=t;
	}
	void sew(const SVGArt &other) {
		stringstream ss;
		ss << "<g><g transform='translate("<<(-w/2) <<" 0)'>"<<picture<<"</g>" 
		   <<"<g transform='translate("<<(w/2) <<" 0)'>"<<other.picture
		   <<"</g></g>";
		w+=other.w;
		picture=ss.str();
	}
};

class LitA:public SVGArt{
	public:
  LitA() {
	 picture="<rect width='12' height='12' x='-6' y='-6' style='fill:#00ffff' />\n"
             "<circle style='fill:#ff0000' cx='-3' cy='-3' r='3' />\n";
     w=12;
     h=12;
  }
};

class LitB:public SVGArt{
	public:
  LitB() {
		picture="<rect width='12' height='12' x='-6' y='-6' style='fill:#00ffff' />\n"
         "<rect width='6' height='6' x='-6' y='-6' style='fill:#008000' />\n";
        w=12;
        h=12;
  }
};
