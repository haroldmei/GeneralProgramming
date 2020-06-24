#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <list>
#include <iterator>
#include <stdlib.h>

using namespace std;

// Complete the decode_all_words function below.
vector<string> decode_all_words(string code) {

	string letters = "OPTIVER";
	vector<string> codes({"---", ".--.", "-", "..", "...-", ".", ".-."});
	vector<string> results;
	for (size_t i = 0; i < codes.size(); i++)
	{
		if (code.find(codes[i]) == 0)
		{
			char c = letters[i];
			if (codes[i].size() != code.size()) 
			{
				vector<string> result = decode_all_words(code.substr(codes[i].size()));
				for (size_t j = 0; j < result.size(); j++)
					results.push_back(c + result[j]);
			}
			else
				results.push_back(string(1, c)); //leaf
		}
	}

	sort(results.begin(), results.end(), less<string>());
	return results;
}

string myadd(string num1, string num2)
{
	int c = 0;
	if (num1.size() < num2.size())
		swap(num1, num2);

	num2 = string(num1.size() - num2.size(), '0') + num2;
	transform(num1.rbegin(), num1.rend(), num2.rbegin(), num1.rbegin(), [&c](char a, char b) { 
		int sum = (a - '0') + (b - '0') + c;
		c = sum / 10;
		return to_string(sum % 10)[0];
	});

	if (c)
		num1 = '1' + num1;

	return num1;
}

string mymul(string num1, char n)
{
	int c = 0;
	string result;
	for_each(num1.rbegin(), num1.rend(), [&c, n, &result](char a) {
		int res = (a - '0') * (n - '0') + c;
		c = res / 10;
		result = to_string(res % 10) + result;
	});
	result = c ? to_string(c) + result : result;
	return result;
}

// Complete the multiply function below.
string multiply(string num1, string num2) {

	if (num2.size() > num1.size())
		swap(num1, num2);

	string result = "0";
	size_t len = num2.size();
	for (int i = 0; i < num2.size(); i++)
	{
		string cur = mymul(num1, num2[len - 1 - i]);
		cur = cur + string(i, '0');
		result = myadd(result, cur);
	}
	return result;
}

int calc_days_between(string start, string end) {

	int d1 = stoi(start.substr(0, 2));
	int d2 = stoi(end.substr(0, 2));
	int m1 = stoi(start.substr(2, 2));
	int m2 = stoi(end.substr(2, 2));
	int y1 = stoi(start.substr(4));
	int y2 = stoi(end.substr(4));
	
	auto leap = [](int y)->bool {
		return (y % 2) || !(y % 10); 
	};

	auto days = [](int d, int m, int y)->int { 
		return (m - 1) * 28 - m / 6 + d;
	};

	int past_days1 = days(d1, m1, y1);
	int past_days2 = days(d2, m2, y2);
	if (y1 == y2)
		return past_days2 - past_days1;

	int leapday = leap(y1) ? 1 : 0;
	int result = 668 - past_days1 + leapday;
	for (int i = y1 + 1; i < y2; i++)
	{
		result += leap(i) ? 669 : 668;
	}
	result += past_days2;
	return result;
}

int main_optiver()
{
	//string ss = myadd("1234567","5678");
	//string ss1 = mymul("12345", '4');
	string ss2 = multiply("82948596", "69448948596");
	cout << ss2 << endl;

	int days = calc_days_between("01012010", "10012010");
	cout << days << endl;

	days = calc_days_between("01012010", "01022010");
	cout << days << endl;

	days = calc_days_between("01012010", "01012011");
	cout << days << endl;

	return 0;
}
