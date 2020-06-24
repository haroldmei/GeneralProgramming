#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <iterator>

using namespace std;

class A
{
public:
	A() : A(1) {}
	A(int a) {}
};
class B :public A
{

};

int main0()
{
	try
	{

	}
	catch (A a)
	{

	}
	catch (B a)
	{

	}
	catch (...)
	{

	}
	return 0;
}
