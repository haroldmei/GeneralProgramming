
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

// Complete the biggerIsGreater function below.
// pqommldkafmnwzidydgjghxcbnwyjdxpvmkztdfmcxlkargafjzeye
string biggerIsGreater(string w) {

	for (int i = w.size() - 1; i > 0; i--)
	{
		if (w[i] > w[i - 1])
		{
			sort(w.begin() + i, w.end());
			int j = i;
			while (w[j] <= w[i - 1])
				j++;

			swap(w[j], w[i - 1]);
			return w;
		}
		else
		{
			//cout<<w<<endl;
		}
	}
	return "no answer";
}

string timeInWords(int h, int m) {

	vector<string> ones({ "zero", "one", "two", "three", "four", "five" , "six" , "seven" , "eight" , "nine" });
	vector<string> teens({ "ten", "eleven" , "twelve" , "thirteen" , "fourteen" , "fifteen" , "sixteen" , "seventeen" , "eighteen" , "ninteen" });
	vector<string> tens({ "", "ten", "twenty" , "thirty" , "fourty" , "fifty" , "sixty" , "seventy" , "eighty" , "ninty" , "one handred" });

	auto readnum = [ones, teens, tens](int n) -> string {
		int ten = n / 10;
		int one = n % 10;
		if (ten == 0)
		{
			return ones[one];
		}
		else if (ten == 1)
		{
			return teens[one];
		}
		else if (one)
		{
			return string(tens[ten] + " " + ones[one]);
		}
		else
		{
			return string(tens[ten]);
		}
	};

	string hr = readnum(h);
	string hr1 = readnum(h + 1);
	if (!m)
	{
		return hr + " o' clock";
	}
	else if (m == 15)
	{
		return "quarter past " + hr;
	}
	else if (m == 30)
	{
		return "half past " + hr;
	}
	else if (m == 45)
	{
		return "quarter to " + hr1;
	}
	else if (m == 1)
	{
		return readnum(m) + " minute past " + hr;
	}
	else if (m == 59)
	{
		return readnum(60 - m) + " minute to " + hr1;
	}
	else if (m < 30)
	{
		return readnum(m) + " minutes past " + hr;
	}
	else if (m > 30)
	{
		return readnum(60 - m) + " minutes to " + hr1;
	}
	else
	{
		//what else?
		return "what else?";
	}
}

// Complete the gridSearch function below.
string gridSearch(vector<string> G, vector<string> P) {

	return "";
}

string sortString(string ss)
{
	auto pred = [](const char &a, const char & b) { return a > b; };

	ostream_iterator<char> out_it(std::cout, ", ");

	priority_queue<char, vector<char>, less<char>> pq;	// 
	for_each(ss.begin(), ss.end(), [&pq](const char& c) {
		pq.push(c);
	});

	string result;
	while (!pq.empty())
	{
		char c = pq.top();
		//out_it++ = c;
		result += c;
		pq.pop();
	}

	cout << result << endl;
	return result;
}


string stackString(string ss)
{
	stack<char> st;

	for_each(ss.begin(), ss.end(), [&st](const char& c) {
		st.push(c);
	});

	string result;
	while (!st.empty())
	{
		char c = st.top();
		//out_it++ = c;
		result += c;
		st.pop();
	}

	cout << result << endl;
	return result;
}

string listString(string ss)
{
	list<char> ls;

	for_each(ss.begin(), ss.end(), [&ls](const char& c) {
		ls.push_back(c);
	});

	string result;
	for (auto it = ls.rbegin(); it != ls.rend(); it++)
	{
		result.push_back(*it);
	}

	cout << result << endl;
	return result;
}
int main1()
{
	//ifstream fin("input01.txt", std::ifstream::in);
	//ofstream fout("output01.txt", std::ofstream::out);
	//while (!fin.eof())
	//{
	//	string w;
	//	getline(fin, w);
	//
	//	string result = biggerIsGreater(w);
	//
	//	fout << result << "\n";
	//}
	//
	//fin.close();
	//fout.close();

	//string str = timeInWords(5,45);
	//cout << str << endl;

	string rs = sortString("input01.txt");
	rs = stackString("input01.txt");
	rs = listString("input01.txt");

	return 0;
}
