/*Given an integer n, return 1 - n in lexicographical order.

For example, given 13, return: [1,10,11,12,13,2,3,4,5,6,7,8,9].

Please optimize your algorithm to use less time and space. The input size may be as large as 5,000,000.

med
*/

class Solution {
public:
    vector<int> lexicalOrder(int n) {
        vector<int> v;
        int i = 1;
        while (1){
            if (i <= n){
                v.push_back(i);
                i *= 10;
            }
            else{
                i /= 10;
                int j = 1;
                while ((i % 10 + j) < 10 && i + j <= n){
                    v.push_back(i + j);
                    j++;
                }

                i = i / 10 + 1;
                while (!(i % 10))
                    i /= 10;

                if (i == 1)
                    break;
            }
        }
        return v;
    }
};