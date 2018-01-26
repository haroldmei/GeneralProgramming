/*Given an array of integers, every element appears three times except for one, which appears exactly once. Find that single one.

Note:
Your algorithm should have a linear runtime complexity. Could you implement it without using extra memory?
*/


public class Solution {
    public int singleNumber(int[] nums) {
        HashMap<Integer, Integer> bucket = new HashMap<Integer, Integer>();
        for (int a : nums)
        {
            Integer cur = bucket.get(a);
            if (cur == null)
            {
                bucket.put(a, new Integer(1));
            }
            else
            {
                cur++;
                if (cur == 3)
                {
                    bucket.remove(a);
                }
                else
                {
                    bucket.put(a, cur);
                }
            }
        }
        Iterator itr = bucket.keySet().iterator();
        //if (itr.hasNext())
        return (Integer) itr.next();
    }
}