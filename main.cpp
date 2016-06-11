#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <vector>
#include <stack>
#include <map>
using std::string;
using std::ostringstream;
using std::cout;
using std::endl;
using std::vector;
using std::stack;
using std::map;
#include "log.h"
#include "ztype.h"
#include "zio.h"
#include "zconvert.h"
#include "cstruct.h"
#include "preprocess.h"
#include "desugar.h"

void start(vector<string> strs);

int main(int argc, char** argv) {
	ByteBuffer buffer = ByteBuffer(1024);
	readFile(fopen("f:\\test.zlh","r"),&buffer);
	cout<<buffer.info()<<endl;
	vector<string> strs = split(buffer);
	cout<<strs.size()<<endl;
	strs = trimAll(strs);
	initPreprocess();
	start(strs);
	return 0 ;
}

void start(vector<string> strs){
	preprocess(strs,string("<sourcecode>"));
	collect_required_element(hseq);
	start_desugar();
}
