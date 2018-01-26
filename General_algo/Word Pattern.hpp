/*Given a pattern and a string str, find if str follows the same pattern.

Here follow means a full match, such that there is a bijection between a letter in pattern and a non-empty word in str.

Examples:
pattern = "abba", str = "dog cat cat dog" should return true.
pattern = "abba", str = "dog cat cat fish" should return false.
pattern = "aaaa", str = "dog cat cat dog" should return false.
pattern = "abba", str = "dog dog dog dog" should return false.
Notes:
You may assume pattern contains only lowercase letters, and str contains lowercase letters separated by a single space.
*/

class Solution {
public:

	bool wordPattern(string pattern, string str) {

		vector<string> tokens;

		int b = 0;
		int e = 0;
		do{
			e = str.find(" ", b);
			string tmp = str.substr(b, e - b);
			tokens.push_back(tmp);
			b = e + 1;
		} while (e > 0);

		if (pattern.size() != tokens.size())
			return false;

		map<char, string> dict;
		map<string, char> dict1;
		for (int i = 0; i < pattern.size(); i++){
			dict[pattern[i]] = tokens[i];
			dict1[tokens[i]] = pattern[i];
		}

		return dict.size() == dict1.size();
	}
};