/*Given a triangle, find the minimum path sum from top to bottom. Each step you may move to adjacent numbers on the row below.

For example, given the following triangle
[
     [2],
    [3,4],
   [6,5,7],
  [4,1,8,3]
]
The minimum path sum from top to bottom is 11 (i.e., 2 + 3 + 5 + 1 = 11).

Note:
Bonus point if you are able to do this using only O(n) extra space, where n is the total number of rows in the triangle.
*/

public class Solution {
    
    public int minimumTotal(List<List<Integer>> triangle) {

		if (triangle == null || triangle.size() == 0)
			return 0;
		
    	int depth = triangle.size();
        int[] results = new int[depth];
		
		for (int j = 0; j < depth; j++)
		{
			results[j] = ((List<Integer>)triangle.get(depth - 1)).get(j);
		}

		for (int i = depth - 1; i > 0; i--)
		{
			for (int j = 0; j < i; j++)
			{
				results[j] = Math.min(results[j],results[j + 1]) 
					+ ((List<Integer>)triangle.get(i - 1)).get(j);
			}
			System.out.println(results[i - 1]);
		}
		return results[0];
    }
}