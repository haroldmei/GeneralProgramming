/*Your task is to calculate ab mod 1337 where a is a positive integer and b is an extremely large positive integer given in the form of an array.

Example1:

a = 2
b = [3]

Result: 8
Example2:

a = 2
b = [1,0]

Result: 1024
*/


class Solution {
public:

    int superPow(int a, vector<int>& b) {
        a %= 1337;
        if (a <= 0)
            return 0;

        int tmp = 1;
        map<int, int> res;
        int count = 0;
        do{
            map<int, int>::iterator itr = res.find(tmp);
            if (itr == res.end())
                res[tmp] = ++count;
            else{
                count = res.size() - res[tmp] + 1;
                break;
            }
            tmp *= a;
            int tt = tmp % 1337;

            tmp = tt;
        } while (1);

        int cur = 0;
        for (int i = 0; i < b.size(); i++){
            cur = cur * 10 + b[i];
            if (cur >= count)
                cur %= count;
        }

        tmp = 1;
        while (cur--){
            tmp *= a;
            tmp %= 1337;
        }
        return tmp;
    }
};