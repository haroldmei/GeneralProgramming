#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <exception>
#include <algorithm>


#include <iterator>

using namespace std;

class StrVec {
public:
	StrVec() : // the allocator member is default initialized
		elements(nullptr), first_free(nullptr), cap(nullptr) {
	}

	StrVec(const StrVec&); // copy constructor

	StrVec& operator=(const StrVec&); // copy assignment

	~StrVec(); // destructor

	void push_back(const std::string&); // copy the element

	size_t size() const { 
		return first_free - elements; 
	} 
	size_t capacity() const { 
		return cap - elements; 
	} 
	std::string* begin() const { 
		return elements; 
	} 
	std::string* end() const { 
		return first_free; 
	}
	// ...
private:
	std::allocator<std::string> alloc; // allocates the elements

	// used by the functions that add elements to the StrVec
	void chk_n_alloc() { 
		if (size() == capacity()) 
			reallocate(); 
	}

	// utilities used by the copy constructor, assignment operator, and destructor
	std::pair<std::string*, std::string*> alloc_n_copy(const std::string*, const std::string*);

	void free(); // destroy the elements and free the space

	void reallocate(); // get more space and copy the existing elements
	void output() { cout << first_free - elements << " " << cap - elements << endl; }
	std::string* elements; // pointer to the first element in the array
	std::string* first_free; // pointer to the first free element in the array
	std::string* cap; // pointer to one past the end of the array
};

StrVec& StrVec::operator=(const StrVec& rhs)
{
	// call alloc_n_copy to allocate exactly as many elements as in rhs
	auto data = alloc_n_copy(rhs.begin(), rhs.end()); free(); elements = data.first; first_free = cap = data.second; return *this;
}

void StrVec::reallocate()
{
	// we'll allocate space for twice as many elements as the current size
	auto newcapacity = size() ? 2 * size() : 1;

	// allocate new memory
	auto newdata = alloc.allocate(newcapacity);

	// move the data from the old memory to the new
	auto dest = newdata; // points to the next free position in the new array
	auto elem = elements; // points to the next element in the old array
	for (size_t i = 0; i != size(); ++i) 
		alloc.construct(dest++, std::move(*elem++));
	free(); // free the old space once we've moved the elements

	// update our data structure to point to the new elements
	elements = newdata; first_free = dest; cap = elements + newcapacity;
}

pair<string*, string*>
StrVec::alloc_n_copy(const string* b, const string* e)
{
	// allocate space to hold as many elements as are in the range
	auto data = alloc.allocate(e - b);

	// initialize and return a pair constructed from data and
	// the value returned by uninitialized_copy
	return { data, uninitialized_copy(b, e, data) };
}

void StrVec::push_back(const string& s)
{
	chk_n_alloc(); // ensure that there is room for another element

	// construct a copy of s in the element to which first_free points
	alloc.construct(first_free++, s);
	output();
}

StrVec::~StrVec() { 
	free(); 
}

void StrVec::free()
{
	// may not pass deallocate a 0 pointer; if elements is 0, there's no work to do
	if (elements) {

		// destroy the old elements in reverse order
		for (auto p = first_free; p != elements; /* empty */)
			alloc.destroy(--p);

		alloc.deallocate(elements, cap - elements);
	}
}
class A
{
public:
	A(int a) { std::cout << "A::A()\n"; }
	//A() { std::cout << "A::A()\n"; }
	virtual void Foo() { std::cout << "A::Foo()\n"; }
};

class B : public A
{
public:
	B():A(0) { std::cout << "B::B()\n"; }
	//B(int a) : A(a){}
	virtual void Foo() { std::cout << "B::Foo()\n"; }
};

int main2()
{
	A& a = dynamic_cast<A&>(*(new B));
	return 0;
}