#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <list>
#include <iterator>
#include <map>
#include <numeric>
#include <stdlib.h>

using namespace std;

int solution0(vector<int>& A) {
	// write your code in C++14 (g++ 6.2.0)
	set<int> a(A.begin(), A.end());
	int max = *a.rbegin();

	auto ri = a.rbegin();
	for (; ri != a.rend(); ri++)
	{
		if (*ri < 0)
			break;
	}
	auto first = ri.base();
	if (first == a.end() || *first > 1)
		return 1;

	for (int i = 1; i < max + 1; i++)
	{
		if (i != *first++)
			return i;
	}

	return max + 1;
}

int solution1(vector<int>& A) {
	// write your code in C++14 (g++ 6.2.0)
	int sum = 0;
	for_each(A.begin(), A.end(), [&sum](int a) { sum += a; });
	int minus = A.size() - sum;
	return abs(sum - minus) / 2;
}

int solution2(string& S) {
	// write your code in C++14 (g++ 6.2.0)
	string::iterator first = find(S.begin(), S.end(), '1');
	auto last = make_reverse_iterator(first);
	if (last == S.rbegin())
		return 0;

	int count = 0;
	for (auto c = S.rbegin(); c != last; count++)
	{
		if (*c != '0')
			* c = '0';
		else
			c++;		
	}
	return count - 1;
}

void frac(int& a, int& b)
{
	int c = a < b ? a : b;
	for (int i = c; i > 1; i--)
	{
		if (a % i == 0 && b % i == 0)
		{
			a /= i;
			b /= i;
			frac(a, b);
		}
	}
}
int solution3(vector<int>& X, vector<int>& Y) {
	// write your code in C++14 (g++ 6.2.0)
	int sz = X.size();
	for (int i = 0; i < sz; i++)
	{
		frac(X[i], Y[i]);
	}

	map<tuple<int,int>, int> result;
	for (int i = 0; i < sz; i++)
	{
		auto cur = make_tuple(X[i], Y[i]);
		if (result.find(cur) == result.end())
			result[cur] = 0;
		result[cur]++;
	}
	
	int max = 0;
	for (auto a : result)
	{
		if (a.second > max)
			max = a.second;
	}
	return max;
}

int solution(vector<int>& X, vector<int>& Y) 
{
	int max = 0;

	while (X.size())
	{
		int sz = X.size();
		vector<int> processed({ 0 });
		for (int j = 1; j < sz; j++)
		{
			if (X[0] * Y[j] == Y[0] * X[j])
			{
				processed.push_back(j);
			}
		}
		if (max < processed.size())
			max = processed.size();

		for (auto ri = processed.rbegin(); ri != processed.rend(); ri++)
		{
			X.erase(X.begin() + *ri);
			Y.erase(Y.begin() + *ri);
		}
	}
	return max;
}

int main_dolby()
{
	cout << "test" << endl;
	vector<int> A({ 1,0,0,0,1,1 });
	int sum = accumulate(A.begin(), A.end(), 0);

	int result = 0;
	vector<int> a1({ 1,2,3,4,0 });
	vector<int> b1({ 2,3,6,8,4 });
	result = solution(a1, b1);
	cout << result << endl;

	vector<int> a2({ 3,3,4 });
	vector<int> b2({ 5,4,3 });
	result = solution(a2, b2);
	cout << result << endl;

	vector<int> a3({ 4,4,7,1,2 });
	vector<int> b3({ 4,4,8,1,2 });
	result = solution(a3, b3);
	cout << result << endl;

	vector<int> a4({1,2,3,1,2 });
	vector<int> b4({ 2,4,6,5,10 });
	result = solution(a4, b4);
	cout << result << endl;
	
	int maximum = 2000000000;
	int minimum = 1999999990;
	vector<int> a5({ 1,2,3,1,2 });
	vector<int> b5({ 2,4,6,5,10 });
	for (int i = 0; i < 100000; i++)
	{
		int rn1 = rand() % (maximum - minimum + 1) + minimum;
		int rn2= rand() % (maximum - minimum + 1) + minimum;
		a5.push_back(rn1);
		b5.push_back(rn2);
	}
	result = solution(a5, b5);
	//cout << result << endl;
	return 0;
}
