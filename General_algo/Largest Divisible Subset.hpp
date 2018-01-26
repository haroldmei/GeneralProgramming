/*Given a set of distinct positive integers, find the largest subset such that every pair (Si, Sj) of elements in this subset satisfies: Si % Sj = 0 or Sj % Si = 0.

If there are multiple solutions, return any subset is fine.

Example 1:

nums: [1,2,3]

Result: [1,2] (of course, [1,3] will also be ok)
Example 2:

nums: [1,2,4,8]

Result: [1,2,4,8]
*/

class Solution {
public:
    vector<int> largestDivisibleSubset(vector<int>& nums) {

		int len = nums.size();
		vector<int> pre(len, -1);
		vector<int> cnt(len, 1);
		sort(nums.begin(), nums.end());

		for (int i = 1; i < len; i++){

			int max = 0;
			int idxMax = -1;
			for (int j = i - 1; j >= 0; j--){

				if (nums[i] % nums[j] == 0){
					
					if (cnt[j] > max){
						idxMax = j;
						max = cnt[idxMax];
					}
				}
			}

			if (idxMax >= 0){
				pre[i] = idxMax;
				cnt[i] = cnt[idxMax] + 1;
			}
		}

		int max = 0;
		int idxMax = -1;
		for (int i = 0; i < len; i++){
			if (cnt[i] > max){
				max = cnt[i];
				idxMax = i;
			}
		}
		vector<int> res(max, 0);
		for (int i = max; i > 0; i--){
			res[i - 1] = nums[idxMax];
			idxMax = pre[idxMax];
		}

		return res;
	}
};