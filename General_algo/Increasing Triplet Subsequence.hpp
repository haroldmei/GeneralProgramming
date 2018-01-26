/*Given an unsorted array return whether an increasing subsequence of length 3 exists or not in the array.

Formally the function should:
Return true if there exists i, j, k 
such that arr[i] < arr[j] < arr[k] given 0 ≤ i < j < k ≤ n-1 else return false.
Your algorithm should run in O(n) time complexity and O(1) space complexity.

Examples:
Given [1, 2, 3, 4, 5],
return true.

Given [5, 4, 3, 2, 1],
return false.
*/


class Solution {
public:
    bool increasingTriplet(vector<int>& nums) {
        int i = 0;
        int j = 1;
        int k = 2;
        int len = nums.size();
        while(i < len && j < len && k < len){
            if (nums[j] > nums[i]){
                if (nums[k] > nums[j])
                    return true;
                else{
                    if (nums[k] > nums[i])
                        j = k;
                        
                    k++;
                    if (k == len){
                        j++; k = j + 1;
                    }
                }
                
                if (j == len){
                    i++; j = i + 1; k = j + 1;
                }
            }
            else{
                i++; j = i + 1; k = j + 1;
            }
        }
        return false;
    }
};