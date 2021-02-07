#include <iostream>
#include <thread>

void hello()
{
	std::cout<<"Hello concurrent world"<<std::endl;
}

int main(int argc, char **argv) {
	std::thread t(hello);
	while(true) { };
}

