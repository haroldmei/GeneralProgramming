/*Given numRows, generate the first numRows of Pascal's triangle.

For example, given numRows = 5,
Return

[
     [1],
    [1,1],
   [1,2,1],
  [1,3,3,1],
 [1,4,6,4,1]
]
*/

class Solution {
public:
    vector<vector<int>> generate(int numRows) {
		vector<vector<int>> res;
		if (numRows < 1)
			return res;

		res.push_back(vector<int>({ 1 }));
		for (int i = 1; i < numRows; i++){
			res.push_back(vector<int>(i + 1, 1));
			for (int j = 0; j < i + 1; j++){
				res[i][j] = (j == 0 || j == i) ? 1 : res[i - 1][j - 1] + res[i - 1][j];
			}
		}
		return res;
	}
};