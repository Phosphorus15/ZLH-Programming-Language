struct _object{
	int a;
	_object(){
		a = 0;
	}
	void increase(){
		this->a++;
	}
};

void increase(_object* o){
	o->a++;
}


int main(int argc,char ** argv){
	return 0;
}

