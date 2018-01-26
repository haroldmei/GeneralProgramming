
/*We are playing the Guess Game. The game is as follows:

I pick a number from 1 to n. You have to guess which number I picked.

Every time you guess wrong, I'll tell you whether the number I picked is higher or lower.

However, when you guess a particular number x, and you guess wrong, you pay $x. You win the game when you guess the number I picked.

Example:

n = 10, I pick 8.

First round:  You guess 5, I tell you that it's higher. You pay $5.
Second round: You guess 7, I tell you that it's higher. You pay $7.
Third round:  You guess 9, I tell you that it's lower. You pay $9.

Game over. 8 is the number I picked.

You end up paying $5 + $7 + $9 = $21.
Given a particular n ≥ 1, find out how much money you need to have to guarantee a win.
*/

class Solution {
public:
    int getMoneyAmount(int n) {

        // lenth 1 -> 0; lenth 2 -> select smaller one; lenth 3 -> select middle;
        vector<vector<int>> dp(n + 1, vector<int>(n + 1, 0x7fffffff));
        for (int i = 1; i <= n; i++)dp[i][i] = 0;
        for (int i = 1; i <= n - 1; i++)dp[i][i + 1] = i;
        for (int i = 1; i <= n - 2; i++)dp[i][i + 2] = i + 1;

        // from lenth 3, fill the array out;
        for (int nn = 3; nn <= n; nn++)
        for (int m = 1; m <= n - nn; m++){
            int mini = 0x7fffffff;
            for (int i = m + 2; i < m + nn; i++){
                int l = dp[m][i - 1];
                int r = dp[i + 1][m + nn];
                mini = min(mini, max(l, r) + i);
            }
            dp[m][m + nn] = mini;
        }
        return dp[1][n];
    }
};