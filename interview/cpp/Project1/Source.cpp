#include <iostream>
#include <string>

#define STATIC_CHECK(expr) { char unnamed[(expr)?1:0]; }

template<class To, class From>
To safe_reinterpret_cast(From f)
{
	STATIC_CHECK(sizeof(f) <= sizeof(To));
	return reinterpret_cast<To>(f);
}

int main()
{
	std::string name;
	std::cout << "What is your name? ";

	int a = 10;
	char c = safe_reinterpret_cast<chat>(a);

	getline(std::cin, name);
	std::cout << "Hello, " << name << "!\n";
}
