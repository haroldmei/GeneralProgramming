/*
Given two integers representing the numerator and denominator of a fraction, return the fraction in string format.

If the fractional part is repeating, enclose the repeating part in parentheses.

For example,

Given numerator = 1, denominator = 2, return "0.5".
Given numerator = 2, denominator = 1, return "2".
Given numerator = 2, denominator = 3, return "0.(6)".
*/

class Solution {
public:
    string fractionToDecimal(int aa, int bb){

		string res;
		long long a = aa;
		long long b = bb;

		if (!b)
			return res;

		string str = "";
		if (a < 0 && b > 0 || a > 0 && b < 0){
			str = "-";
		}

		a = a < 0 ? -a : a;
		b = b < 0 ? -b : b;

		int cur = 0;
		long long remain = a % b;
		//vector<long long> occurence = {remain};
		map<long long, int> occurence;
		occurence[remain] = 0;


		while (remain) {
			remain *= 10;

			cur = remain / b;
			res += to_string(cur);
			remain %= b;

			map<long long, int>::iterator itr = occurence.find(remain);
			if (occurence.end() != itr){
				res.insert(occurence[remain], "(");
				res += ")";
				break;
			}
			occurence[remain] = occurence.size() - 1;// in visual studio 2013 the '-1' part is not needed.
		}

		str += to_string(a / b);
		if (res.size()){
			str += ".";
			str += res;
			return str;
		}
			
		return str;
	}
};