#include <vector>
#include <stdexcept>
#include <iostream>

class TreeCount
{
public:
	static int count(const std::vector<int>& tree)
	{
		int sz = tree.size();
		int num = 0;
		std::vector<int> count(sz, 0);
		for (int i = 0; i < sz; i++)
		{
			if (tree[i] == -1)
				continue;

			count[tree[i]]++;
		}

		for (int i = 0; i < sz; i++)
		{
			if (count[i] > 1 && tree[i] != -1)
			{
				num++;
			}
		}
		return num;
	}
};


#ifndef RunTests
int main_1(int argc, const char* argv[])
{
	std::vector<int> tree;
	tree.push_back(1);
	tree.push_back(3);
	tree.push_back(1);
	tree.push_back(-1);
	tree.push_back(3);
	tree.push_back(2);

	std::cout << TreeCount::count(tree);
	return 0;
}
#endif