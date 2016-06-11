#define INTEGER 1
#define FLOATING 2

struct rdigit{
	bool i;
	union{
		long long ll;
		long double ld;
	}content;
	rdigit(DWORD mode){
		switch(mode){
			case INTEGER:
				i=true;
				content.ll = 0;
				break;
			case FLOATING:
				i=false;
				content.ld = 0.0;
				break;
			default:
				i=true;
		}
	}
	void add(long double d){
		if(i){
			content.ll+=d;
		}else{
			content.ld+=d;
		}
	}
	void multiply(){
		if(i){
			content.ll*=10;
		}else{
			content.ld*=10;
		}
	}
};

bool c_error;
bool c_floating;

const void* fastConvert(string str){
	if(!isdigit(str)){
		c_error = true;
		return str.c_str();
	}
	c_error = false;
	rdigit* d;
	if(contains(str,'.')){
		d = new rdigit(FLOATING);
	}else{
		d = new rdigit(INTEGER);
	}
	for(int x=0;x<str.length()&&str[x]!='.';x++){
		d->add((double)str[x]-48);
		d->multiply();
	}
	c_floating = false;
	if(d->i)
		return &d->content.ll;
	long double pre = 0.0;
	for(int x=str.length()-1;str[x]!='.';x--){
		pre+=str[x]-48;
		pre/=10;
	}
	d->add(pre);
	c_floating = true;
	return &d->content.ld;
}

string fastConvert(long number){
	string buffer;
	bool neg = false;
	if(number<0){
		neg = true;
		number = -number;
	}
	while(number>10){
		buffer = string()+((char)(number%10 + 48))+buffer;
		number /= 10;
	}
	buffer = string()+((char)(number%10 + 48))+buffer;
	if(neg){
		buffer = string("-")+buffer;
	}
	return buffer;
}
