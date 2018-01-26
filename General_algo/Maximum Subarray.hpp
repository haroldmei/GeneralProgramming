/*Find the contiguous subarray within an array (containing at least one number) which has the largest sum.

For example, given the array [-2,1,-3,4,-1,2,1,-5,4],
the contiguous subarray [4,-1,2,1] has the largest sum = 6.

click to show more practice.

More practice:
If you have figured out the O(n) solution, try coding another solution using the divide and conquer approach, which is more subtle.
*/

class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        if (nums.size() == 1)
            return nums[0];

        int len = nums.size();
        vector<int> sums = nums;
        for (int i = 1; i < len; i++){
            sums[i] = sums[i] + sums[i - 1];
        }

        int minSum = sums[0];
        int maxSum = sums[0];
        int b = 0;
        int e = 0;
        for (int i = 1; i < len; i++){
            minSum = min(minSum, sums[i - 1]);
            if (sums[i] > 0)
                maxSum = max(maxSum, sums[i] - (minSum < 0 ? minSum : 0));
            else
                maxSum = max(maxSum, sums[i] - minSum);
        }

        return maxSum;
    }
};