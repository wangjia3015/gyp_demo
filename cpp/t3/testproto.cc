#include <stdio.h>
#include <string>
#include "msg.pb.h"

int main () {
	lm::helloworld hw;
	hw.set_id(1);
	hw.set_str("hello world");
	hw.set_opt(2015);
	std::string buf;
	hw.SerializeToString(&buf);


	lm::helloworld hw2;
	
	hw2.ParseFromString(buf);
	printf("hw2 str %s opt %d id = %d \n", hw2.str().c_str(), hw2.opt(), hw2.id());

	return 0;
}
