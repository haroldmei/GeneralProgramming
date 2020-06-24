#include <vector>
#include <stdexcept>
#include <iostream>
#include <set>

class Sum4
{
public:
	/**
	*   \brief Accepts a sequence of integers.
	*
	**/
	void addLast(const std::vector<int>& list)
	{
		theLast5.insert(theLast5.end(), list.begin(), list.end());
		if (theLast5.size() < 5)
			return;

		for (int i = 0; i < theLast5.size() - 4; i++)
		{
			int sum = theLast5[i] + theLast5[i + 1] + theLast5[i + 3] + theLast5[i + 4];
			elements.insert(sum);
		}

		// theLast5.erase(theLast5.begin(), theLast5.begin());
		// for (int i = 0; i < theLast5.size() - 5; i++)
		// {
		// 	theLast5.
		// }
	}

	/**
	*   \brief Quickly respond whether there exists a sub-sequence of 5 consecutive numbers
		 excluding the middle number that add to a given number within the stream.
	*
	**/
	bool containsSum4(int sum)
	{
		return elements.find(sum) != elements.end();
	}

private:
	std::vector<int> theLast5;	// keep track of 5 element
	std::set<int> elements;		// the sum of 4
};

#ifndef RunTests
int main_2(int argc, const char* argv[])
{
	Sum4 s;

	std::vector<int> first;
	first.push_back(1);
	first.push_back(2);
	first.push_back(3);
	first.push_back(4);
	first.push_back(5);

	s.addLast(first);

	std::cout << s.containsSum4(12) << '\n';  // true

	std::vector<int> second;
	second.push_back(6);
	second.push_back(7);
	s.addLast(second);

	std::cout << s.containsSum4(14) << '\n';  // false
	std::cout << s.containsSum4(20) << '\n';  // true
	return 0;
}
#endif