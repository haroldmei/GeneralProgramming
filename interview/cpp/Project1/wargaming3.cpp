

#include <set>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <tuple>
#include <list>
#include <iostream>

using namespace std;

struct key_hash : public std::unary_function<tuple<int, int>, std::size_t>
{
	std::size_t operator()(const tuple<int, int>& k) const
	{
		return std::get<0>(k) ^ std::get<1>(k);
	}
};
int solution(vector< vector<int> >& A) {
	// write your code in C++14 (g++ 6.2.0)

	unordered_map < tuple<int, int>, list<tuple<int, int>>, key_hash> countries;

	for (int a1 = 0; a1 < A.size(); a1++)
	{
		for (int a2 = 0; a2 < A[0].size(); a2++)
		{
			auto cur = make_tuple(a1, a2);
			countries[cur] = list<tuple<int, int>>({ cur });
		}
	}

	for (int a2 = 0; a2 < A[0].size(); a2++)
	{
		for (int a1 = 0; a1 < A.size(); a1++)
		{
			tuple<int, int> cur = make_tuple(a1, a2);
			if (a1 < A.size() - 1 && A[a1][a2] == A[a1 + 1][a2])
			{
				auto cur1 = make_tuple(a1 + 1, a2);
				countries[cur].merge(countries[cur1]);
			}
			if (a2 < A[0].size() - 1 && A[a1][a2] == A[a1][a2 + 1])
			{
				auto cur1 = make_tuple(a1, a2 + 1);
				countries[cur].merge(countries[cur1]);
			}
			if (a2 > 0 && A[a1][a2] == A[a1][a2 - 1])
			{
				auto cur1 = make_tuple(a1, a2 - 1);
				countries[cur].merge(countries[cur1]);
			}
			if (a1 > 0 && A[a1][a2] == A[a1 - 1][a2])
			{
				auto cur1 = make_tuple(a1 - 1, a2);
				countries[cur].merge(countries[cur1]);
			}
		}
	}

	set<set<tuple<int, int>>> results;
	for (auto a = countries.begin(); a != countries.end(); a++)
	{
		if (a->second.size() == 0)
			continue;

		set<tuple<int, int>> cur(a->second.begin(), a->second.end());
		results.insert(cur);
	}
	return results.size();
}


int main()
{
	vector< vector<int> > A = { {1,1,2},{3,3,2},{3,3,2},{2,2,2},{4,4,5},{6,4,5} };
	int result = solution(A);
	cout << result << endl;
	return 0;
}