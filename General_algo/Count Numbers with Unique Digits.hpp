/*Given a non-negative integer n, count all numbers with unique digits, x, where 0 ≤ x < 10n.

Example:
Given n = 2, return 91. (The answer should be the total numbers in the range of 0 ≤ x < 100, excluding [11,22,33,44,55,66,77,88,99])

Med
*/

class Solution {
public:
    int countNumbersWithUniqueDigits(int n) {
        if (!n)return 1;
        vector<int> arr(10, 10);
        for (int i = 1; i < 10; i++)
        {
            int delta = 1;

            for (int j = 0; j < i; j++)delta *= (9 - j);
            for (int j = i; j < 10; j++)arr[j] += 9 * delta;
        }

        return arr[n < 10 ? n - 1 : 9];
    }
};