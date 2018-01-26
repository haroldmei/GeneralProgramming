/*Follow up for "Unique Paths":

Now consider if some obstacles are added to the grids. How many unique paths would there be?

An obstacle and empty space is marked as 1 and 0 respectively in the grid.

For example,
There is one obstacle in the middle of a 3x3 grid as illustrated below.

[
  [0,0,0],
  [0,1,0],
  [0,0,0]
]
The total number of unique paths is 2.
*/

public class Solution {
    public int uniquePathsWithObstacles(int[][] obstacleGrid) {
		if (obstacleGrid == null 
			|| obstacleGrid.length == 0 
			|| obstacleGrid[0].length == 0)
			return 0;
		
		int m = obstacleGrid.length;
        int n = obstacleGrid[0].length;
		
        int[][] result = new int[m][n];
		result[0][0] = obstacleGrid[0][0] == 1 ? 0 : 1;
        for(int i = 0; i < m; i++)
        {
        	if (i > 0)
	            result[i][0] = obstacleGrid[i][0] == 1 ? 0 : result[i - 1][0];
			
            for (int j = 1; j < n; j++)
            {
            	if (obstacleGrid[i][j] == 1)
					result[i][j] = 0;
				else
	                result[i][j] = i == 0 ? result[i][j - 1] : 
	                    (result[i - 1][j] + result[i][j - 1]);
            }
        }

        return result[m - 1][n - 1];
    }
}