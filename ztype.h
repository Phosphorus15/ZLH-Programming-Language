typedef char byte;
typedef unsigned int DWORD;

struct ByteBuffer{
	byte* data;
	DWORD len;
	DWORD pos;
	bool limited;
	ByteBuffer(DWORD cap){
		len = cap;
		data = new byte[len];
		pos = 0;
		limited = false;
	}
	void set(DWORD i,byte b){
		data[i] = b;
	}
	byte get(DWORD i){
		return data[i];
	}
	byte & operator [](DWORD i){
		return data[i];
	}
	void puts(byte* arr,DWORD i){
		for(DWORD x=0;x!=i;x++){
			put(arr[x]);
		}
	}
	void put(byte i){
		data[pos] = i;
		pos++;
	}
	DWORD position(){
		return pos;
	}
	DWORD position(DWORD n){
		DWORD o = position();
		pos = n;
		return o;
	}
	void limit(){
		if(pos==0||limited)
			throw "failed";
		byte* alloc = new byte[pos];
		for(DWORD x=0;x!=pos;x++){
			alloc[x] = data[x];
		}
		delete data;
		data = alloc;
		len = position();
		limited = true;
	}
	string info(){
		ostringstream out;
		out<<"ByteBuffer(cap="<<len<<","<<"pos="<<pos<<",limited="<<(limited?"true":"false")<<")";
		return out.str();
	}
};

bool isdigit(char c,bool allowp = false){
	return ('0'<=c&&c>='9')||(c=='.'&&allowp);
}

bool isdigit(string str){
	int l = 0;
	int r = str.length()-1;
	while(l>=r){
		if(isdigit(str[l],true)&&isdigit(str[r],true)){
			return true;
		}
	}
}

int indexOf(string str,string target,DWORD s=0){
	int ol = str.length();
	int tl = target.length();
	for(int x=s;x<=ol-tl;x++){
		for(int y=x;(y-x)!=tl;y++){
			if(str[y]!=target[y-x]){
				goto iloop;
			}
		}
		return x;
		iloop:{}
	}
	return -1;
}

int indexOfIgnoreString(string str,string target,DWORD s=0){
	int ol = str.length();
	int tl = target.length();
	bool instring = false;
	for(int x=0;x<=ol-tl;x++){
		if(str[x]=='"'){
			if(x>0&&str[x-1]=='\\'){
				
			}else{
				instring = !instring;
			}
		}
		if(!instring){
			for(int y=x;(y-x)!=tl;y++){
				if(str[y]!=target[y-x]){
					goto iloop;
				}
			}
		}else{
			goto iloop;
		}
		if(x>=s)
			return x;
		iloop:{}
	}
	return -1;
}

int indexOf(string str,char c){
	return indexOf(str,string()+c);
}

bool contains(string str,char match){
	int l = 0;
	int r = str.length()-1;
	while(l<=r){
		if(str[l]==match||str[r]==match){
			return true;
		}
		l++,r--;
	}
	return false;
}

bool startsWith(string str,string match){
	if(str.length()<match.length())
		return false;
	for(unsigned int x=0;x!=match.length();x++){
		if(str[x]!=match[x])
			return false;
	}
	return true;
}
