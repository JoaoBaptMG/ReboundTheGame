#include <iostream>
#include <string>
#include "Coroutine.hpp"

using namespace std;

string handle(Coroutine<string,string>& coro, string value)
{
	int n = 0;
	cout << value << " <- " << n << endl;
	n++;
	value = coro.yield("coroutine from main#1");  
	cout << value << " <- " << n << endl;
	n++;
	value = coro.yield("coroutine from main#2"); 
	cout << value << " <- " << n << endl;
	n++;
	value = coro.yield("coroutine from main#3"); 
	cout << value << " <- " << n << endl;
	return "coroutine from main#4";
}

int main()
{
	// Small text
	Coroutine<string,string> coro(handle);
    
	int m = 7;
	auto value = coro.resume("main from coroutine#1");
	cout << value << " -> " << m << endl;
	m--;
	value = coro.resume("main from coroutine#2");
	cout << value << " -> " << m << endl;
	m--;
	value = coro.resume("main from coroutine#3");
	cout << value << " -> " << m << endl;
	m--;
	value = coro.resume("main from coroutine#4");
	cout << value << " -> " << m << endl;

	return 0;
}
