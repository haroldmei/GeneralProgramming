/*Given a non-empty string s and a dictionary wordDict containing a list of non-empty words, determine if s can be segmented into a space-separated sequence of one or more dictionary words. You may assume the dictionary does not contain duplicate words.

For example, given
s = "leetcode",
dict = ["leet", "code"].

Return true because "leetcode" can be segmented as "leet code".

UPDATE (2017/1/4):
The wordDict parameter had been changed to a list of strings (instead of a set of strings). Please reload the code definition to get the latest changes.
*/


public class Solution {

    boolean wordBreak(String s, Set<String> dict) 
    {
        String s2 = "#" + s;
        int len = s2.length();
        boolean[] possible = new boolean[len];
        
        possible[0] = true;
        for(int i = 1; i < len; ++i)
        {
            for(int k = 0; k < i; ++k)
            {
            	String sub = s2.substring(k + 1, i + 1);
                possible[i] = possible[k] 
                    && dict.contains(sub);
                if(possible[i])
                    break;
            }
        }
        
        return possible[len - 1];
    }
}