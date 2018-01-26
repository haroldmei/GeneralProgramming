/*Given an array nums, there is a sliding window of size k which is moving from the very left of the array to the very right. You can only see the k numbers in the window. Each time the sliding window moves right by one position.

For example,
Given nums = [1,3,-1,-3,5,3,6,7], and k = 3.

Window position                Max
---------------               -----
[1  3  -1] -3  5  3  6  7       3
 1 [3  -1  -3] 5  3  6  7       3
 1  3 [-1  -3  5] 3  6  7       5
 1  3  -1 [-3  5  3] 6  7       5
 1  3  -1  -3 [5  3  6] 7       6
 1  3  -1  -3  5 [3  6  7]      7
Therefore, return the max sliding window as [3,3,5,5,6,7].

Note: 
You may assume k is always valid, ie: 1 ≤ k ≤ input array's size for non-empty array.

Follow up:
Could you solve it in linear time?
*/

import java.util.*;
public class Solution {
    public int[] maxSlidingWindow(int[] nums, int k) {

		if (nums == null)
			return null;

		if (nums.length == 0)
			return nums;
		
		int[] result = new int[nums.length - k + 1];
		TreeSet<Map.Entry<Integer, Integer>> bucket = 
			new TreeSet<Map.Entry<Integer, Integer>>(new Comparator<Map.Entry<Integer, Integer>>(){
			public int compare(Map.Entry<Integer, Integer> e1, Map.Entry<Integer, Integer> e2)
			{
				if (e1.getKey().intValue() > e2.getKey().intValue())
				{
					return 1;
				}
				else if (e1.getKey().intValue() == e2.getKey().intValue())
				{
					return e1.getValue().intValue() > e2.getValue().intValue() ? -1 : e1.getValue().intValue() == e2.getValue().intValue() ? 0 : 1;
				}
				else
				{	
					return -1;
				}
			}
		});
		
		for (int i = 0; i < k; i++)
		{
			bucket.add(new AbstractMap.SimpleEntry(nums[i], i));
		}
		result[0] = bucket.last().getKey().intValue();

		for (int i = k; i < nums.length; i++)
		{
			bucket.remove(new AbstractMap.SimpleEntry(nums[i - k], i - k));
			
			bucket.add(new AbstractMap.SimpleEntry(nums[i], i));
			result[i - k + 1] = bucket.last().getKey().intValue();
		}

		return result;
    }
}