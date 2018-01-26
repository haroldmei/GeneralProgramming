/*A robot is located at the top-left corner of a m x n grid (marked 'Start' in the diagram below).

The robot can only move either down or right at any point in time. The robot is trying to reach the bottom-right corner of the grid (marked 'Finish' in the diagram below).

How many possible unique paths are there?


Above is a 3 x 7 grid. How many possible unique paths are there?
*/


public class Solution {
    public int uniquePaths(int m, int n) {
    	if (m < 1 || n < 1)
			return 0;
		
        int[][] result = new int[m][n];
		for(int i = 0; i < m; i++)
		{
			result[i][0] = 1;
			for (int j = 1; j < n; j++)
			{
				result[i][j] = i == 0 ? 1 : 
					(result[i - 1][j] + result[i][j - 1]);
			}
		}

		return result[m - 1][n - 1];
    }
}