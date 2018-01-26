/*
Given an array of integers, return indices of the two numbers such that they add up to a specific target.

You may assume that each input would have exactly one solution, and you may not use the same element twice.

Example:
Given nums = [2, 7, 11, 15], target = 9,

Because nums[0] + nums[1] = 2 + 7 = 9,
return [0, 1].
*/

class Solution {
public:
    vector<int> twoSum(vector<int>& list, int sum) {
        
		std::map<int, std::vector<int>> dgt;
		for (int i = 0; i < list.size(); i++)
			dgt[list[i]].push_back(i);

		for (auto it1 = dgt.begin(); it1 != dgt.end(); it1++)
		{
			auto it2 = dgt.find(sum - it1->first);
			if (it2 == dgt.end())
				continue;

			if (it2 != it1)
				return std::vector<int>({it1->second.front(), it2->second.front()});
			else
				return std::vector<int>({it1->second.front(), it1->second.back()});
		}
		return std::vector<int>({-1, -1});
    }
};