/*Given two arrays, write a function to compute their intersection.

Example:
Given nums1 = [1, 2, 2, 1], nums2 = [2, 2], return [2].

Note:
Each element in the result must be unique.
The result can be in any order.
*/

class Solution {
public:
    vector<int> intersection(vector<int>& nums1, vector<int>& nums2) {

        vector<int> res;
        sort(nums1.begin(), nums1.end());
        sort(nums2.begin(), nums2.end());
        int i = 0, j = 0;
        while (i < nums1.size() && j < nums2.size()){
            if (nums1[i] == nums2[j]){
                if (res.size() && res.back() != nums1[i] || !res.size())
                    res.push_back(nums1[i]);
                i++, j++;
            }
            else if (nums1[i] < nums2[j])
                i++;
            else
                j++;
        }
        return res;
    }
};