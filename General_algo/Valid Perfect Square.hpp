/*Given a positive integer num, write a function which returns True if num is a perfect square else False.

Note: Do not use any built-in library function such as sqrt.

Example 1:

Input: 16
Returns: True
Example 2:

Input: 14
Returns: False
*/

class Solution {
public:
    bool isPerfectSquare(int num) {
        int b = 1;
        int e = 46340;
        while (b < e){
            int m = (b + e) >> 1;
            int tmp = m * m;
            if (tmp > num)
                e = m;
            else if (tmp < num)
                b = m + 1;
            else
                return true;
        }
        return false;
    }
};