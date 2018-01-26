/*Given an array of integers and an integer k, find out whether there are two distinct indices i and j in the array such that nums[i] = nums[j] and the absolute difference between i and j is at most k.
*/Contains Duplicate II

class Solution {
public:
	bool containsNearbyDuplicate(vector<int>& nums, int k) {
		if (k > nums.size())
			k = nums.size();

		vector<int>::iterator itr = nums.begin();
		set<int> a(itr, itr + k);
		if (a.size() != k)
			return true;

		while (itr + k < nums.end()){

			a.insert(*(itr + k));
			if (a.size() != k + 1)
				return true;

			a.erase(*itr);
			itr++;
		}
		return false;
	}
};