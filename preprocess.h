map<string,string> prep;

map<string,vector<string> > pheader;

vector<string> pmain;

stack<_cstack*> pstack;

vector<string> hseq;

map<string,macro> macros;
map<string,macro> pmacros;

void initPreprocess(){
	prep[string("version")] = string("0");
	prep[string("bits")] = string("16");
}

void npre(vector<string> strs);

bool macroR(string exp){
	vector<string> param = split(exp,' ',3);
	if(param.size()==2){
		if(prep.count(param[0])==0){
			return false;
		}else{
			return prep[param[0]].compare(param[1])==0;
		}
	}else{
		signed char p;//free up space
		if((p=comparation(param[0]))!=0xf){
			#warning TODO
			error("preserved function",pstack.top()->line);
			void* a;
			void* b;
			switch(p){
				case 0:
					if(prep.count(param[1])==0){
						return false;
					}else{
						return prep[param[1]].compare(param[2])==0;
					}
			}
		}else{
			param = split(exp,' ',2);
			if(prep.count(param[0])==0){
				return false;
			}else{
				return prep[param[0]].compare(param[1])==0;
			}
		}
	}
}

string macroP(string str){
	int d=-1;
	while((d=indexOfIgnoreString(str,string("$"),d+1))!=-1){
		//cout<<"punc found - "<<str<<endl; abandon debug message
		string buffer;
		bool param = false;
		for(int x=d;x<str.length();x++){
			if(str[x]=='('){
				param = true;
			}
			if(((str[x]==' '||str[x]==',')&&!param)||str[x]==')'){
				goto out;
			}
			buffer+=str[x];
		}
		out:{}
		buffer = buffer.substr(1);
		if(!trim(buffer).compare("")){
			error("macro replacement failed : null macro name",pstack.top()->line);
		}else{
			if(param){
				#warning TODO
				warn("unsupported operation",pstack.top()->line);
			}else{
				if(macros.count(buffer)==0){
					error((buffer+" no such macro : ").c_str(),pstack.top()->line);
				}else{
					string p = str.substr(0,d);
					string s = str.substr(d+buffer.length()+1);
					str = p + macros[buffer].body + s;
					debug((string("macro replaced : ")+buffer).c_str(),pstack.top()->line);
				}
			}
		}
	}
	return str;
}

void preprocess(vector<string> strs,string filename){
	_cstack data;
	stack<bool> rg;
	rg.push(true);
	data.line = 0;
	data.file = filename;
	pstack.push(&data);
	string tmp;
	vector<string> result;
	for(vector<string>::iterator it = strs.begin();it!=strs.end();it++){
		tmp = *it;
		data.line++;
		if(startsWith(tmp,string("#endif"))){
			rg.pop();
			continue;
		}else if(startsWith(tmp,string("#elif"))){
			if(tmp.length()<=6){
				error("invalid 'elif' identifier",data.line);
			}else{
				tmp = tmp.substr(6);
				if(macroR(tmp)&&(!rg.top())){
					rg.pop();
					rg.push(true);
				}else{
					rg.pop();
					rg.push(false);
				}
			}
			continue;
		}else if(startsWith(tmp,string("#else"))){
			bool current = rg.top();
			rg.pop();
			rg.push(!current);
			continue;
		}else{
			if(!rg.top()){
				continue;
			}
		}
		if(tmp[0]=='#'){
			tmp = tmp.substr(1);
			if(startsWith(tmp,string("using"))){
				if(tmp.length()<=6){
					error("invalid 'using' identifier",data.line);
				}else{
					tmp = tmp.substr(6);
					tmp+=".zlh";
					string org = tmp;
					tmp=string("lib\\")+tmp;
					debug((string("trying to open library : ")+tmp).c_str(),data.line);
					FILE* f = fopen(tmp.c_str(),"r");
					if(f==NULL){
						error("no such library",data.line);
					}
					ByteBuffer* buffer = readFile(f);
					_cstack nextg;
					nextg.file = org;
					nextg.line = 0;
					pstack.push(&nextg);
					npre(trimAll(split(*buffer)));
				}
			}else if(startsWith(tmp,string("process"))||startsWith(tmp,string("sys"))){
				error("security error : critical pre-process command",data.line);
			}else if(startsWith(tmp,string("define"))){
				if(tmp.length()<=7){
					error("invalid 'define' identifier",data.line);
				}else{
					tmp = tmp.substr(7);
					vector<string> param = split(tmp,' ',1);
					if(param.size()<2){
						error("invalid 'define' identifier - too few arguments",data.line);
					}
					prep[param[0]] = param[1];
					debug((string("new pre-processing constant defined : ")+param[0]).c_str(),data.line);
				}
			}else if(startsWith(tmp,string("ifdef"))){
				if(tmp.length()<=6){
					error("invalid 'ifdef' identifier",data.line);
				}else{
					tmp = tmp.substr(6);
					if(prep.count(tmp)==0){
						rg.push(false);
					}else{
						rg.push(true);
					}
				}
			}else if(startsWith(tmp,string("ifndef"))){
				if(tmp.length()<=7){
					error("invalid 'ifndef' identifier",data.line);
				}else{
					tmp = tmp.substr(7);
					if(prep.count(tmp)==0){
						rg.push(true);
					}else{
						rg.push(false);
					}
				}
			}else if(startsWith(tmp,string("if"))){
				if(tmp.length()<=3){
					error("invalid 'if' identifier",data.line);
				}else{
					tmp = tmp.substr(3);
					if(macroR(tmp)){
						rg.push(true);
					}else{
						rg.push(false);
					}
				}
			}else if(startsWith(tmp,string("error"))){
				if(tmp.length()<=6){
					error("no specified message",data.line);
				}
				tmp = tmp.substr(6);
				error(tmp.c_str(),data.line);
			}else if(startsWith(tmp,string("warning"))){
				if(tmp.length()<=8){
					warn("no specified message",data.line);
				}
				tmp = tmp.substr(8);
				warn(tmp.c_str(),data.line);
			}else if(startsWith(tmp,string("macro"))){
				if(tmp.length()<=6){
					error("invalid 'macro' identifier",data.line);
				}else{
					vector<string> param = split(tmp,' ',2);
					if(param.size()==1){
						error("invalid 'macro' identifier - too few arguments",data.line);
					}else{
						macro m;
						if(contains(param[0],'(')&&contains(param[1],')')){
							
						}else{
							m.name = param[0];
							m.param = vector<string>();
						}
						if(param[1][param[1].length()-1]=='\\'){
							m.body = param[1];
							#warning TODO
							error("unsupported feature",data.line);
						}else{
							m.body = param[1];
						}
						debug((string("new hidden macro defined : ")+param[0]).c_str(),data.line);
						pmacros[m.name] = m;//preprocess.h	[Error] invalid preprocessing directive
					}
				}
			}else{
				warn("ignored invalid pre-process command",data.line);
			}
		}else{
			result.push_back(macroP(tmp));
			continue ;
		}
	}
	pall(result),debug("preprocess accomplished");
	pheader[filename] = result;
	hseq.push_back(filename);
}

void process(vector<string> strs){
	_cstack data = *pstack.top();
	debug((string("start of preprocess : ")+data.file).c_str());
	pstack.pop();
	pstack.push(&data);
	stack<bool> rg;
	rg.push(true);
	string tmp;
	vector<string> result;
	for(vector<string>::iterator it = strs.begin();it!=strs.end();it++){
		tmp = *it;
		data.line++;
		if(startsWith(tmp,string("#endif"))){
			rg.pop();
			continue;
		}else if(startsWith(tmp,string("#elif"))){
			if(tmp.length()<=6){
				error("invalid 'elif' identifier",data.line);
			}else{
				tmp = tmp.substr(6);
				if(macroR(tmp)&&(!rg.top())){
					rg.pop();
					rg.push(true);
				}else{
					rg.pop();
					rg.push(false);
				}
			}
			continue;
		}else if(startsWith(tmp,string("#else"))){
			bool current = rg.top();
			rg.pop();
			rg.push(!current);
			continue;
		}else{
			if(!rg.top()){
				continue;
			}
		}
		if(tmp[0]=='#'){
			tmp = tmp.substr(1);
			if(startsWith(tmp,string("using"))){
				if(tmp.length()<=6){
					error("invalid 'using' identifier",data.line);
				}else{
					tmp = tmp.substr(6);
					tmp+=".zlh";
					string org = tmp;
					tmp=string("lib\\")+tmp;
					debug((string("trying to open library : ")+tmp).c_str(),data.line);
					FILE* f = fopen(tmp.c_str(),"r");
					if(f==NULL){
						error("no such library",data.line);
					}
					ByteBuffer* buffer = readFile(f);
					_cstack nextg;
					nextg.file = org;
					nextg.line = 0;
					pstack.push(&nextg);
					npre(trimAll(split(*buffer)));
				}
			}else if(startsWith(tmp,string("process"))){
				if(tmp.length()<=8){
					error("invalid 'process' identifier",data.line);
				}else{
					tmp = tmp.substr(8);
					tmp+=".zlh";
					string org = tmp;
					tmp=string("lib\\")+tmp;
					debug((string("trying to process policy-library : ")+tmp).c_str(),data.line);
					FILE* f = fopen(tmp.c_str(),"r");
					if(f==NULL){
						error("no such library",data.line);
					}
					ByteBuffer* buffer = readFile(f);
					_cstack nextg;
					nextg.file = org;
					nextg.line = 0;
					pstack.push(&nextg);
					process(trimAll(split(*buffer)));
				}
			}else if(startsWith(tmp,string("sys"))){
				//TODO
			}else if(startsWith(tmp,string("define"))){
				if(tmp.length()<=7){
					error("invalid 'define' identifier",data.line);
				}else{
					tmp = tmp.substr(7);
					vector<string> param = split(tmp,' ',1);
					if(param.size()<2){
						error("invalid 'define' identifier - too few arguments",data.line);
					}
					prep[param[0]] = param[1];
					debug((string("new pre-processing constant defined : ")+param[0]).c_str(),data.line);
				}
			}else if(startsWith(tmp,string("ifdef"))){
				if(tmp.length()<=6){
					error("invalid 'ifdef' identifier",data.line);
				}else{
					tmp = tmp.substr(6);
					if(prep.count(tmp)==0){
						rg.push(false);
					}else{
						rg.push(true);
					}
				}
			}else if(startsWith(tmp,string("ifdnef"))){
				if(tmp.length()<=7){
					error("invalid 'ifdef' identifier",data.line);
				}else{
					tmp = tmp.substr(7);
					if(prep.count(tmp)==0){
						rg.push(true);
					}else{
						rg.push(false);
					}
				}
			}else if(startsWith(tmp,string("if"))){
				if(tmp.length()<=3){
					error("invalid 'if' identifier",data.line);
				}else{
					tmp = tmp.substr(3);
					if(macroR(tmp)){
						rg.push(true);
					}else{
						rg.push(false);
					}
				}
			}else if(startsWith(tmp,string("error"))){
				if(tmp.length()<=6){
					error("no specified message",data.line);
				}
				tmp = tmp.substr(6);
				error(tmp.c_str(),data.line);
			}else if(startsWith(tmp,string("warning"))){
				if(tmp.length()<=8){
					warn("no specified message",data.line);
				}
				tmp = tmp.substr(8);
				warn(tmp.c_str(),data.line);
			}else if(startsWith(tmp,string("macro"))){
				if(tmp.length()<=6){
					error("invalid 'macro' identifier",data.line);
				}else{
					tmp = tmp.substr(6);
					vector<string> param = split(tmp,' ',2);
					if(param.size()==1){
						error("invalid 'macro' identifier - too few arguments",data.line);
					}else{
						macro m;
						if(contains(param[0],'(')&&contains(param[1],')')){
							
						}else{
							m.name = param[0];
							m.param = vector<string>();
						}
						if(param[1][param[1].length()-1]=='\\'){
							m.body = param[1];
							error("unsupported feature",data.line);
						}else{
							m.body = param[1];
						}
						debug((string("new macro defined : ")+param[0]).c_str(),data.line);
						macros[m.name] = m;
					}
				}
			}else{
				warn("ignored invalid pre-process command",data.line);
			}
		}else{
			result.push_back(macroP(tmp));
			continue ;
		}
	}
	pall(result);
	pheader[data.file] = result;
	pstack.pop();
	debug((string("end of process : ")+data.file).c_str());
}

void npre(vector<string> strs){
	_cstack data = *pstack.top();
	debug((string("start of preprocess : ")+data.file).c_str());
	pstack.pop();
	pstack.push(&data);
	stack<bool> rg;
	rg.push(true);
	string tmp;
	vector<string> result;
	for(vector<string>::iterator it = strs.begin();it!=strs.end();it++){
		tmp = *it;
		data.line++;
		if(startsWith(tmp,string("#endif"))){
			rg.pop();
			continue;
		}else if(startsWith(tmp,string("#elif"))){
			if(tmp.length()<=6){
				error("invalid 'elif' identifier",data.line);
			}else{
				tmp = tmp.substr(6);
				if(macroR(tmp)&&(!rg.top())){
					rg.pop();
					rg.push(true);
				}else{
					rg.pop();
					rg.push(false);
				}
			}
			continue;
		}else if(startsWith(tmp,string("#else"))){
			bool current = rg.top();
			rg.pop();
			rg.push(!current);
			continue;
		}else{
			if(!rg.top()){
				continue;
			}
		}
		if(tmp[0]=='#'){
			tmp = tmp.substr(1);
			if(startsWith(tmp,string("using"))){
				if(tmp.length()<=6){
					error("invalid 'using' identifier",data.line);
				}else{
					tmp = tmp.substr(6);
					tmp+=".zlh";
					string org = tmp;
					tmp=string("lib\\")+tmp;
					debug((string("trying to open library : ")+tmp).c_str(),data.line);
					FILE* f = fopen(tmp.c_str(),"r");
					if(f==NULL){
						error("no such library",data.line);
					}
					ByteBuffer* buffer = readFile(f);
					_cstack nextg;
					nextg.file = org;
					nextg.line = 0;
					pstack.push(&nextg);
					npre(trimAll(split(*buffer)));
				}
			}else if(startsWith(tmp,string("process"))){
				if(tmp.length()<=8){
					error("invalid 'process' identifier",data.line);
				}else{
					tmp = tmp.substr(8);
					tmp+=".zlh";
					string org = tmp;
					tmp=string("lib\\")+tmp;
					debug((string("trying to process policy-library : ")+tmp).c_str(),data.line);
					FILE* f = fopen(tmp.c_str(),"r");
					if(f==NULL){
						error("no such library",data.line);
					}
					ByteBuffer* buffer = readFile(f);
					_cstack nextg;
					nextg.file = org;
					nextg.line = 0;
					pstack.push(&nextg);
					process(trimAll(split(*buffer)));
				}
			}else if(startsWith(tmp,string("sys"))){
				//TODO
			}else if(startsWith(tmp,string("define"))){
				if(tmp.length()<=7){
					error("invalid 'define' identifier",data.line);
				}else{
					tmp = tmp.substr(7);
					vector<string> param = split(tmp,' ',1);
					if(param.size()<2){
						error("invalid 'define' identifier - too few arguments",data.line);
					}
					prep[param[0]] = param[1];
					debug((string("new pre-processing constant defined : ")+param[0]).c_str(),data.line);
				}
			}else if(startsWith(tmp,string("ifdef"))){
				if(tmp.length()<=6){
					error("invalid 'ifdef' identifier",data.line);
				}else{
					tmp = tmp.substr(6);
					if(prep.count(tmp)==0){
						rg.push(false);
					}else{
						rg.push(true);
					}
				}
			}else if(startsWith(tmp,string("ifdnef"))){
				if(tmp.length()<=7){
					error("invalid 'ifdef' identifier",data.line);
				}else{
					tmp = tmp.substr(7);
					if(prep.count(tmp)==0){
						rg.push(true);
					}else{
						rg.push(false);
					}
				}
			}else if(startsWith(tmp,string("if"))){
				if(tmp.length()<=3){
					error("invalid 'if' identifier",data.line);
				}else{
					tmp = tmp.substr(3);
					if(macroR(tmp)){
						rg.push(true);
					}else{
						rg.push(false);
					}
				}
			}else if(startsWith(tmp,string("error"))){
				if(tmp.length()<=6){
					error("no specified message",data.line);
				}
				tmp = tmp.substr(6);
				error(tmp.c_str(),data.line);
			}else if(startsWith(tmp,string("warning"))){
				if(tmp.length()<=8){
					warn("no specified message",data.line);
				}
				tmp = tmp.substr(8);
				warn(tmp.c_str(),data.line);
			}else if(startsWith(tmp,string("macro"))){
				if(tmp.length()<=6){
					error("invalid 'macro' identifier",data.line);
				}else{
					tmp = tmp.substr(6);
					vector<string> param = split(tmp,' ',2);
					if(param.size()==1){
						error("invalid 'macro' identifier - too few arguments",data.line);
					}else{
						macro m;
						if(contains(param[0],'(')&&contains(param[1],')')){
							
						}else{
							m.name = param[0];
							m.param = vector<string>();
						}
						if(param[1][param[1].length()-1]=='\\'){
							m.body = param[1];
							error("unsupported feature",data.line);
						}else{
							m.body = param[1];
						}
						debug((string("new macro defined : ")+param[0]).c_str(),data.line);
						macros[m.name] = m;
					}
				}
			}else{
				warn("ignored invalid pre-process command",data.line);
			}
		}else{
			result.push_back(macroP(tmp));
			continue ;
		}
	}
	pall(result);
	pheader[data.file] = result;
	hseq.push_back(data.file);
	pstack.pop();
	debug((string("end of preprocess : ")+data.file).c_str());
}

struct allocation_policy{
	string _byte;
	string _word;
	string _dword;
	string _qword;
	string _string;
	string _var;
	allocation_policy(){
		_byte = pmacros[string("allocate_byte")].body;
		_word = pmacros[string("allocate_word")].body;
		_dword = pmacros[string("allocate_dword")].body;
		_qword = pmacros[string("allocate_qword")].body;
		_string = pmacros[string("allocate_string")].body;
	}
	string allocateByte(string name){
		return replace(_byte,string("$n"),name);
	}
	string allocateWord(string name){
		return replace(_word,string("$n"),name);
	}
	string allocateDoubleWord(string name){
		return replace(_dword,string("$n"),name);
	}
	string allocateQuadWord(string name){
		return replace(_qword,string("$n"),name);
	}
	string allocateString(string name,int l){
		string str = fastConvert(l);
		return replace(replace(_qword,string("$n"),name),string("$s"),str);
	}
	string allocateStruct(_struct type,string name){
		string all = name+"_struct\n";
		all += rawStructAllocation(type);
		return all;
	}
	string rawStructAllocation(_struct type){
		#warning TODO
	}
};
