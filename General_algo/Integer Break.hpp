/*Given a positive integer n, break it into the sum of at least two positive integers and maximize the product of those integers. Return the maximum product you can get.

For example, given n = 2, return 1 (2 = 1 + 1); given n = 10, return 36 (10 = 3 + 3 + 4).

Note: You may assume that n is not less than 2 and not larger than 58.
*/


class Solution {
public:
    int integerBreak(int n) {
        if (n < 4) return n - 1;
        
        vector<int> res = {0,0,2,3,4,6,9,12,18,27};
        for (int i = 10; i <=n; i++)
        {
            res.push_back(res[i - 3] * 3);
        }
        
        return res[n];
    }
};