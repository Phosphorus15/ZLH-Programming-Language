ByteBuffer* allocateBuffer(){
	return new ByteBuffer(4096);
}

ByteBuffer* readFile(FILE * f,ByteBuffer* buffer = allocateBuffer()){
	int r;
	while(!feof(f)){
		r=fgetc(f);
		buffer->put((byte)r);
	}
	buffer->pos--; //remove EOF mark
	buffer->limit();
	fclose(f);
	return buffer;
}

vector<string> split(ByteBuffer str){
	vector<string> strs;
	string buffer("");
	for(DWORD x=0;x<str.len;x++){
		if(str[x]=='\r'||str[x]=='\n'){
			strs.push_back(buffer);
			buffer.clear();
		}else{
			buffer += str[x];
		}
	}
	strs.push_back(buffer);
	return strs;
}

vector<string> split(string str,char i,DWORD max = -1){
	vector<string> strs;
	string buffer("");
	int c=0;
	for(DWORD x=0;x<str.length();x++){
		if(str[x]==i&&(max==-1||c<max)&&buffer.length()!=0){
			strs.push_back(buffer);
			buffer.clear();
			c++;
		}else{
			buffer += str[x];
		}
	}
	strs.push_back(buffer);
	return strs;
}

string trim(string str){
	while(str.length()>0&&(str[0]==' '||str[0]=='\t')){
		str = str.substr(1);
	}
	while(str.length()>0&&(str[str.length()-1]==' '||str[str.length()-1]=='\t')){
		str = str.substr(0,str.length()-1);
	}
	return str;
}

string attempAnn(string b){
	int last = -1;
	bool is = false;
	for(int x=b.length();x>=0;x--){
		if(b[x]=='"'){
			if(x>0&&b[x-1]=='\\'){
				
			}else{
				is = !is;
			}
		}
		if(b[x]=='\''&&!is){
			if(x>0&&b[x-1]=='\\'){
				
			}else{
				last = x;
			}
		}
	}
	if(last!=-1){
		b = b.substr(0,last);
	}
	return b;
}

vector<string> trimAll(vector<string> v,bool ann = true){
	vector<string>::iterator last = v.begin();
	string tmp;
	for(int x=0;x!=v.size();x++){
		tmp = v[x];
		if(ann)
			tmp = attempAnn(tmp);
		tmp = trim(tmp);
		v[x] = tmp;
	}
	return v;
}

string replaceAll(string str,string o,string t){
	int a;
	while((a=indexOf(str,o))!=-1){
		string p = str.substr(0,a);
		string s = str.substr(a+o.length());
		str = p + t + s;
	}
	return str;
}

string replace(string str,string o,string t){
	int a;
	if((a=indexOf(str,o))==-1){
		return str;
	}else{
		string p = str.substr(0,a);
		string s = str.substr(a+o.length());
		str = p + t + s;
	}
	return str;
}
