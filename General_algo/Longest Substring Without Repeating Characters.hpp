/*Given a string, find the length of the longest substring without repeating characters.

Examples:

Given "abcabcbb", the answer is "abc", which the length is 3.

Given "bbbbb", the answer is "b", with the length of 1.

Given "pwwkew", the answer is "wke", with the length of 3. Note that the answer must be a substring, "pwke" is a subsequence and not a substring.
*/


class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        int len = s.size();
        int i = 0; 
		int j = 0;
        set<char> ext;
        int ans = 0;

        while (j < len && i < len)
        {
            if (ext.find(s[j]) == ext.end())
			{
				ext.insert(s[j++]);
				ans = max(ans, j - i);
			}
			else
			{
				ext.erase(s[i++]);
			}
        }
		return ans;
    }
};