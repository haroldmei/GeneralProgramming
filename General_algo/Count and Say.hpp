/*The count-and-say sequence is the sequence of integers with the first five terms as following:

1.     1
2.     11
3.     21
4.     1211
5.     111221
1 is read off as "one 1" or 11.
11 is read off as "two 1s" or 21.
21 is read off as "one 2, then one 1" or 1211.
Given an integer n, generate the nth term of the count-and-say sequence.

Note: Each term of the sequence of integers will be represented as a string.

Example 1:

Input: 1
Output: "1"
Example 2:

Input: 4
Output: "1211"
*/

class Solution {
public:
    string countAndSay(int n) {
        
		if (n <= 0) return "";
		string res = "1";

		int one = 0; 
		int two = 0;
		int three = 0;
		for (int i = 1; i < n; i++){

			string tmp;
			for (int j = 0; j < res.size(); j++){

				switch (res[j])
				{
				case '1':
					if (!one){
						if (two){
							tmp += to_string(two) + "2";
							two = 0;
						}
						if (three){
							tmp += to_string(three) + "3";
							three = 0;
						}
					}
					one++;
					break;

				case '2':
					if (!two){
						if (one){
							tmp += to_string(one) + "1";
							one = 0;
						}
						if (three){
							tmp += to_string(three) + "3";
							three = 0;
						}
					}
					two++;
					break;

				case '3':
					if (!three){
						if (one){
							tmp += to_string(one) + "1";
							one = 0;
						}
						if (two){
							tmp += to_string(two) + "2";
							two = 0;
						}
					}
					three++;
					break;

				default:
					break;
				}
			}

			if (one){
				tmp += to_string(one) + "1";
				one = 0;
			}
			if (two){
				tmp += to_string(two) + "2";
				two = 0;
			}
			if (three){
				tmp += to_string(three) + "3";
				three = 0;
			}
			res = tmp;
		}

		return res;
    }
};