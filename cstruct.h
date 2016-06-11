#define _typedef(_source,_lstr,_name) _type t;\
t.source = _source;\
t._str = _lstr;\
t._sname = _name;

#define _stypedef(t,_name) t.source = SYSTEM;\
t._sname = _name;

enum code_type{
	ASSIGNMENT,OBJECTIVE_CALL,CALL,CACULATION,CONTROL,ASMB,REFERENCE,ASSEMBLE
};

signed char comparation(string cmp){
	if(!cmp.compare("="))
		return 0;
	if(!cmp.compare(">="))
		return 1;
	if(!cmp.compare(">"))
		return 2;
	if(!cmp.compare("<="))
		return -1;
	if(!cmp.compare("<"))
		return -2;
	else
		return 0xf;
}

struct method{
	string name;
	vector<string> modifier;
	map<string,string> param;
	vector<string> body;
	string rtype;
	string otype;
	string fheader;
	bool inil;
	method(bool nil=false){
		inil = nil;
	}
	void parse_parameters(string str,int l);
};

struct macro{
	string name;
	vector<string> param;
	string body;
};

struct constant{
	string name;
	string type;
	string data;
	bool istruct;
	bool inil;
	constant(bool nil = false){
		istruct = false;
		inil = nil;
	}
};

struct _struct{
	string name;
	vector<constant> data;
	bool itypedef;
	bool inil;
	_struct(bool nil = false){
		itypedef = false;
		if(nil){
			name = string("nil");
		}
		inil = nil;
	}
};

struct _cstack{
	string file;
	int line;
};

struct header{
	string name;
	map<string,constant> constants;
	map<string,_struct> structures;
	map<string,method> methods;
};

enum _type_source{
	SYSTEM=0x1f,TYPEDEF=0x2f,STRUCTURE=0x3f
};

struct _type{
	_type_source source;
	string _sname;
	_struct _str;
	_type(){
		
	}
};

struct combined_header{
	map<string,header> headers;
	void add_header(string str,header h){
		headers[str] = h;
	}
	_struct find_struct(string name){
		for(map<string,header>::iterator it = headers.begin();it!=headers.end();it++){
			if((it->second).structures.count(name)!=0){
				return (it->second).structures[name];
			}
		}
		return _struct(true);
	}
	method find_method(string name){
		for(map<string,header>::iterator it = headers.begin();it!=headers.end();it++){
			if((it->second).structures.count(name)!=0){
				return (it->second).methods[name];
			}
		}
		return method(true);
	}
	method find_objective_method(string name,string type){
		method m;
		if(!(m=find_method(name)).inil){
			if(m.otype.compare(type)!=0){
				return method(true);
			}else{
				return m;
			}
		}else{
			return m;
		}
	}
	constant find_constant(string name){
		for(map<string,header>::iterator it = headers.begin();it!=headers.end();it++){
			if((it->second).structures.count(name)!=0){
				return (it->second).constants[name];
			}
		}
		return constant(true);
	}
	vector<method> list_all(){
		vector<method> methods;
		method m;
		for(map<string,header>::iterator it = headers.begin();it!=headers.end();it++){
			for(map<string,method>::iterator iit = it->second.methods.begin();iit!=it->second.methods.end();iit++){
				m = iit->second;
				m.fheader = it->first;
				methods.push_back(m);
			}
		}
		return methods;
	}
};

struct analysis{
	code_type type;
	int first;
	int second;
	int third;
};

struct seek_result{
	bool contains;
	int start;
	int interm;
	int end;
	void* add;
};

struct compile_trace{
	int line;
	string filename;
	string method;
	compile_trace(){
		line = 0;
	}
};

analysis estimate_analyze(string str){
	analysis ana;
	if(str.compare("end")==0||str.compare("endf")==0){
		ana.type = CONTROL;
		ana.first = 0;
		return ana;
	}
	if(str.compare("asm:")==0){
		ana.type = ASMB;
		return ana;
	}
	bool instring = false;
	bool valid_dot = false;
	int dot_pos;
	stack<char> op;
	for(int x=0;x!=str.length();x++){
		if(str[x]=='"'&&(x!=0&&str[x-1]!='\\')){
			instring = !instring;
		}else if(!instring&&str[x]=='.'){
			valid_dot = true;
			dot_pos = x;
		}else if(!instring&&str[x]=='('){
			if(valid_dot){
				ana.first = dot_pos;
				ana.second = x;
				ana.type = OBJECTIVE_CALL;
			}else{
				ana.first = x;
				ana.type = CALL;
			}
			return ana;
		}else if(!instring&&str[x]=='='&&!valid_dot){
			ana.type = ASSIGNMENT;
			ana.first = x;
			return ana;
		}else if(!instring&&(str[x]=='[')&&!valid_dot){
			ana.type = CONTROL;
			ana.first = x;
			return ana;
		}
	}
	if(valid_dot){
		ana.type = REFERENCE;
		ana.first = dot_pos;
	}else{
		ana.type = ASSEMBLE;
	}
	return ana;
}
