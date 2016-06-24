void info(const char* data,int line){
	if(line==-1){
		fprintf(stdout,"[INFO] %s - unknown\n",data);
		return ;
	}
	fprintf(stdout,"[INFO] %s - line %d\n",data,line);
}

void info(const char* data){
	fprintf(stdout,"[INFO] %s\n",data);
}

void debug(const char* data,int line){
	fprintf(stdout,"[DEBUG] %s - line %d\n",data,line);
}

void debug(const char* data){
	fprintf(stdout,"[DEBUG] %s\n",data);
}

void warn(const char* data,int line){
	fprintf(stdout,"[WARN] %s - line %d\n",data,line);
}

void warn(const char* data){
	fprintf(stdout,"[WARN] %s\n",data);
}

void error(const char* data,int line){
	if(line==-1){
		fprintf(stdout,"[ERROR] %s - unknown\n",data);
	}else
		fprintf(stdout,"[ERROR] %s - line %d\n",data,line);
	exit(0);
}

void pall(vector<string> source){
	for(int x=0;x!=source.size();x++){
		debug(source[x].c_str());
	}
}
