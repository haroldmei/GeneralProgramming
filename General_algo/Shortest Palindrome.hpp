/*Given a string S, you are allowed to convert it to a palindrome by adding characters in front of it. Find and return the shortest palindrome you can find by performing this transformation.

For example:

Given "aacecaaa", return "aaacecaaa".

Given "abcd", return "dcbabcd".
*/

class Solution {
public:
    string shortestPalindrome(string s) {
        
		int len = s.size();
		string ss = s;
		for (int i = 0; i < len / 2; i++){
			char tmp = ss[i];
			ss[i] = ss[len - i - 1];
			ss[len - i - 1] = tmp;
		}

		for (int i = 0; i < len; i++){
			if (!ss.compare(i, len - i, s, 0, len - i)){
				ss = ss.substr(0, i);
				break;
			}
		}

		string rs = ss + s;
		return rs;
    }
};