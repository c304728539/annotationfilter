#include<iostream>


class A {	//class A test 1

};//class A end test 2


int c() {
	char* p = "123123";	//test 3
	char* p2 = "123///123";	//test 4
	/*
	//test 5
	*/
	auto c = '/*'; //should not exist

	/*
	*/
	///*test 6
	

	return 0;
}
