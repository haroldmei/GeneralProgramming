/*Given a string, find the first non-repeating character in it and return it's index. If it doesn't exist, return -1.

Examples:

s = "leetcode"
return 0.

s = "loveleetcode",
return 2.
*/

class Solution {
public:
    int firstUniqChar(string s) {
        std::pair<int, int> counts[26];
        
        int n = s.size();
        for (int i = 0; i < n; i++) {
            int pos = s[i] - 'a';
            counts[pos].first++;
            counts[pos].second = i;
        }
        
        int index = n;
        
        for (int i = 0; i < 26; i++) {
            if (counts[i].first == 1) {
                index = std::min(index, counts[i].second);
            }    
        }
        
        return (index == n) ? -1 : index;
    }
};