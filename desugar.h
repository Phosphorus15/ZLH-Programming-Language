#define STYPE_LENGTH 4

map<string,_type> unitype;

header particle_collection(vector<string> source);

vector<string> mmod;

combined_header cheaders;

void system_type_registry(){
	string *ts =new string[STYPE_LENGTH]{string("byte"),string("word"),string("dword"),string("qword")};
	vector<string> _types(ts,ts+STYPE_LENGTH);
	_type t;
	for(vector<string>::iterator it=_types.begin();it!=_types.end();it++){
		t = _type();
		_stypedef(t,*it);
		unitype[*it] = t;
	}
	mmod.push_back(string("objective"));
	mmod.push_back(string("inline"));
}

bool vcontains(vector<string> v,string c){
	for(vector<string>::iterator it = v.begin();it!=v.end();it++){
		if(c.compare(*it)==0){
			return true;
		}
	}
	return false;
}

void m_modifier_checksum(vector<string> m,string modifier,int l){
	if(!vcontains(mmod,modifier)){
		error((string("no such modifier : ")+modifier).c_str(),l);
	}else if(vcontains(m,modifier)){
		error((string("duplicated modifier : ")+modifier).c_str(),l);
	}
}

void collect_required_element(vector<string> target){
	debug("element collection started");
	system_type_registry();
	for(vector<string>::iterator it = target.begin();it!=target.end();it++){
		cheaders.add_header(*it,particle_collection((pheader[*it])));
	}
}

enum collection{
	NONE=0,METHOD=1,STRUCT=2
};

header particle_collection(vector<string> source){
	collection icollect = NONE;
	method m;
	_struct s;
	header current;
	header h;
	stack<char> p;
	string tmp;
	int i;
	for(int x=0;x!=source.size();x++){
		tmp = source[x];
		if(tmp.length()==0){
			continue;
		}else if(icollect==METHOD){
			if(tmp.compare("endf")==0){
				if(p.size()==0){
					error("unexpected 'endf' stub",-1);
				}else{
					p.pop();
				}
				if(p.size()==0){
					debug((string("method collection finished : ")+m.name).c_str());
					icollect = NONE;
					h.methods[m.name] = m;
					m = method();
				}
			}else{
				m.body.push_back(source[x]);
			}
		}else if(icollect==STRUCT){
			if(tmp.compare("end")==0){
				h.structures[s.name] = s;
				_typedef(STRUCTURE,s,s.name);
				unitype[s.name] = t;
				icollect = NONE;
				debug((string("new structure collected : ")+s.name).c_str(),x);
				continue;
			}
			vector<string> param = trimAll(split(tmp,' '),false);
			if(param.size()==1){
				error("malformed structure member",x);
			}else{
				constant c;
				c.name = param[param.size()-1];
				c.type = param[param.size()-2];
				if(unitype.count(c.type)==0){
					error((string("could not match type name : ")+c.type).c_str(),x);
				}
				c.istruct = (unitype[c.type].source==STRUCTURE);
				s.data.push_back(c);
				#warning Initial value support in-need
			}
		}else{
			if(startsWith(tmp,string("struct"))){
				tmp = tmp.substr(6);
				if(contains(tmp,'=')){
					vector<string> param = trimAll(split(tmp,'='),false);
					s = _struct();
					s.itypedef = true;
					s.name = param[0];
					constant c;
					c.name = string("_preserved_union");
					c.type = param[1];
					c.istruct = false;
					s.data.push_back(c);
					h.structures[param[0]] = s;
					_typedef(TYPEDEF,s,param[0]);
					unitype[param[0]] = t;
					debug((string("type defined ")+s.name+string("-->")+c.type).c_str(),x);
				}else if(tmp[tmp.length()-1]!=':'){
					error("expected ':' after 'struct' identifier",x);
				}else{
					tmp = tmp.substr(0,tmp.length()-1);
					tmp = trim(tmp);
					if(tmp.length()==0){
						error("expected name after 'struct' identifier",x);
					}
					s = _struct();
					s.name = tmp;
					icollect = STRUCT;
				}
			}else if((i=indexOfIgnoreString(tmp,string("__")))!=-1&&indexOfIgnoreString(tmp,string("["))!=-1){
				string prefix = tmp.substr(0,i);
				vector<string> param = trimAll(split(trim(prefix),' '),false);
				if(param.size()==0){
					error("no method modifier",x);
				}
				if(unitype.count(param[0])==0&&!(param[0].compare("void"))==0){
					error("no such return type",x);
				}
				m = method();
				m.rtype = param[0];
				if(param.size()>1){
					for(int id=1;id!=param.size();id++){
						m_modifier_checksum(m.modifier,param[id],id);
						m.modifier.push_back(param[id]);
					}
				}
				string suffix = tmp.substr(i);
				suffix = suffix.substr(2);
				i = indexOfIgnoreString(suffix,string("__"));
				if(i==-1||i==0||i==suffix.length()-2){
					error("non-end method defination",x);
				}
				m.name = suffix.substr(0,i);
				suffix = suffix.substr(i+2);
				if(suffix[suffix.length()-1]!=':'){
					error("non-end method defination",x);
				}
				suffix = trim(suffix.substr(0,suffix.length()-1));
				if(!(suffix[0]=='['&&suffix[suffix.length()-1]==']')){
					error("method defination with no parameter",x);
				}
				suffix = trim(suffix.substr(1,suffix.length()-2));
				if(suffix.length()!=0){
					m.parse_parameters(suffix,x);
				}
				icollect = METHOD;
				p.push('s');
			}else if(startsWith(tmp,string("constant"))){
				if(tmp.length()<10){
					error("malformed constant defination",x);
				}else{
					tmp = tmp.substr(9);
					string type = trim(tmp.substr(0,indexOf(tmp,string(" "))));
					constant c;
					c.type = type;
					if(unitype.count(c.type)==0){
						error("no such type found",x);
						#warning need specified
					}
					c.istruct = (unitype[c.type].source == STRUCTURE);
					tmp = trim(tmp.substr(type.length()));
					if(c.istruct){
						c.name = trim(tmp.substr(0,indexOf(tmp,string("("))));
						tmp = trim(tmp.substr(c.name.length()));
						tmp = tmp.substr(1,tmp.length()-2);
						tmp = replaceAll(tmp,string(" "),string(""));
						c.data = tmp;
					}else{
						vector<string> param = trimAll(split(tmp,'='));
						if(param.size()!=2){
							error("invalid initial data",x);
						}
						c.name = param[0];
						c.data = param[1];
					}
					h.constants[c.name] = c;
					debug((("constant collection finished : ")+c.name).c_str(),x);
				}
			}
		}
	}
	source.clear();
	if(icollect!=NONE){
		error("unended defination",-1);
	}
	return h;
}

void method::parse_parameters(string str,int l){
	vector<string> data = trimAll(split(str,','),false);
	vector<string> tmp;
	for(int x=0;x!=data.size();x++){
		tmp = trimAll(split(data[x],' '),false);
		if(unitype.count(tmp[0])==0){
			error("no such parameter type",l);
		}else{
			if(this->param.count(tmp[1])!=0){
				error("duplicated parameter name",l);
			}else{
				this->param[tmp[1]] = tmp[0];
				if(x==0&&vcontains(this->modifier,string("objective"))){
					this->otype = tmp[0];
				}
			}
		}
	}
}

seek_result seek_decompression_object(string tmp){
	int w;
	seek_result re;
	re.contains = false;
	stack<char> pstack;
	for(int x=0;x!=tmp.length();x++){
		if(tmp[x]=='['){
			pstack.push('[');
		}else if(tmp[x]=='('){
			pstack.push('(');
		}else if(tmp[x]==']'||tmp[x]==')'){
			if(pstack.top()!=tmp[x]){
				//error();
			}
		}
	}
	return re;
}

seek_result seek_objective_call(string tmp){
	seek_result rt;
	bool instr = false;
	stack<char> pstack;
	bool dot=false,fst=false;
	int fst_size = -1;
	for(int x=0;x!=tmp.length();x++){
		if(tmp[x]=='"'&&!(x!=0&&tmp[x-1]!='\\')){
			instr = !instr;
		}
		if(!instr){
			#warning specification in need
			if(tmp[x]==')'){
				if(pstack.size()==0||pstack.top()!='('){
					error("unexpected method invockation end",0);
				}
				if(dot&&fst&&pstack.size()==fst_size){
					rt.end = x;
					rt.contains = true;
					return rt;
				}
				pstack.pop();
			}else if(tmp[x]=='.'){
				//cout<<"dot found : "<<tmp<<endl;
				if(dot){
					if(!fst){
						rt.start = x;
					}
				}else{
					dot = true;
					rt.start = x;
				}
			}else if(tmp[x]=='('){
				//cout<<"l-bracket found : "<<tmp<<endl;
				pstack.push('(');
				if(dot){
					if(!fst){
						//cout<<"established l-bracket record : "<<tmp<<endl;
						fst = true;
						rt.interm = x;
						fst_size = pstack.size();
					}
				}
			}else if(tmp[x]=='['){
				pstack.push('[');
			}else if(tmp[x]==']'){
				if(pstack.size()==0||pstack.top()!='['){
					error("unexpected assertatoin end",0);
				}
				pstack.pop();
			}
		}
	}
	rt.contains = false;
	return rt;
}

method proceed_method(method m){
	vector<string> body = m.body;
	map<string,constant> ivalue;
	string tmp;
	seek_result re;
	for(vector<string>::iterator it = body.begin();it!=body.end();it++){
		tmp = *it;
		//cout<<tmp<<endl;
		/*while((re=seek_objective_call(tmp)).contains){
			cout<<"objective call found : "<<re.start<<"--->"<<re.interm<<"  ["<<tmp<<"]"<<endl;
			break;
		}*/
		while((re=seek_decompression_object(tmp)).contains){
			cout<<"target found : "<<re.start<<"--->"<<re.interm<<"  ["<<tmp<<"]"<<endl;
			break;
		}
	}
	return m;
}

void start_desugar(){
	debug("");
	debug("collecting desugar elements");
	vector<method> methods = cheaders.list_all();
	debug("starting desugar process");
	cout<<methods.size()<<endl;
	for(vector<method>::iterator it = methods.begin();it!=methods.end();it++){
		cout<<"method "<<it->name<<endl;
		*it = proceed_method(*it);
	}
}
