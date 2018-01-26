/*Implement regular expression matching with support for '.' and '*'.

'.' Matches any single character.
'*' Matches zero or more of the preceding element.

The matching should cover the entire input string (not partial).

The function prototype should be:
bool isMatch(const char *s, const char *p)

Some examples:
isMatch("aa","a") → false
isMatch("aa","aa") → true
isMatch("aaa","aa") → false
isMatch("aa", "a*") → true
isMatch("aa", ".*") → true
isMatch("ab", ".*") → true
isMatch("aab", "c*a*b") → true
*/


class Solution {
public:

	bool match(string &s, string &p, int m, int n) {
		if (!m && !n )return true;
		if (m && !n) return false;
		if (!m && n) {
			if (p[n - 1] == '*')
				return match(s, p, 0, n - 2);
			else
				return false;
		}

		if (s[m - 1] == p[n - 1] || s[m - 1] == '.' || p[n - 1] == '.')
			return match(s,p,m-1,n-1);
		if (p[n - 1] == '*'){
			if (p[n - 2] == s[m - 1] || s[m - 1] == '.' || p[n - 2] == '.'){
				return match(s, p, m - 1, n) || match(s, p, m, n - 2);
			}
			else{
				return match(s, p, m, n - 2);
			}
		}
		
		return false;
	}

	bool isMatch(string s, string p) {
		return match(s,p,s.size(),p.size());
	}
};