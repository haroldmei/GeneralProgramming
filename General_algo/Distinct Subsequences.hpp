/*Given a string S and a string T, count the number of distinct subsequences of S which equals T.

A subsequence of a string is a new string which is formed from the original string by deleting some (can be none) of the characters without disturbing the relative positions of the remaining characters. (ie, "ACE" is a subsequence of "ABCDE" while "AEC" is not).

Here is an example:
S = "rabbbit", T = "rabbit"

Return 3.
*/

class Solution {
public:
	int numDistinct(string s, string t){
		/*
		res[i][j]: numDistinct(s[0..i), t[0..j));
		if (s[ i - 1] == t[j - 1])	-> res[i][j] = res[i - 1][j] + res[i - 1][j - 1]
		else						-> res[i][j] = res[i - 1][j] 
		*/
		int slen = s.size();
		int tlen = t.size();

		if (!tlen) 
			return 1;
		if (!slen)
			return 0;

		vector<vector<int>> res(slen + 1, vector<int>(tlen + 1, 0));

		for (int i = 0; i < slen; i++){
			//beautiful assumption, and true assumption!
			res[i][0] = (s[i] == t[0] ? 1 : 0);	
			for (int j = 0; j < tlen; j++){
				res[i + 1][j + 1] = res[i][j + 1] +
					(s[i] == t[j] ? res[i][j] : 0);
			}
		}
		return res[slen][tlen];
	}
};