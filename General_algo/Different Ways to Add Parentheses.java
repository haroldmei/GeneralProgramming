/*Given a string of numbers and operators, return all possible results from computing all the different possible ways to group numbers and operators. The valid operators are +, - and *.


Example 1
Input: "2-1-1".

((2-1)-1) = 0
(2-(1-1)) = 2
Output: [0, 2]


Example 2
Input: "2*3-4*5"

(2*(3-(4*5))) = -34
((2*3)-(4*5)) = -14
((2*(3-4))*5) = -10
(2*((3-4)*5)) = -10
(((2*3)-4)*5) = 10
Output: [-34, -14, -10, -10, 10]
*/

public class Solution {
    public List<Integer> diffWaysToCompute(String input) {

    	if (input == null || input.length() == 0)
    		return null;
    	
    	List<Integer> list = new ArrayList<Integer>();
    	if (!input.contains("+") && !input.contains("-") && !input.contains("*"))
    	{
    		list.add(Integer.valueOf(input));
    		return list;
    	}
    	
        for (int i = 0; i < input.length(); i++)
        {
        	char cur = input.charAt(i);
        	if (cur == '+' || cur == '-' || cur == '*')
        	{
	        	String left = input.substring(0, i);
	        	String right = input.substring(i + 1, input.length());
	        	
	        	List<Integer> leftList = diffWaysToCompute(left);
	        	List<Integer> rightList = diffWaysToCompute(right);
	        	
	        	Iterator iterator = leftList.iterator();
	        	while (iterator.hasNext())
	        	{
	        		Integer lv = (Integer) iterator.next();
	            	Iterator iterator2 = rightList.iterator();
	            	while (iterator2.hasNext())
	            	{
	            		Integer result = 0;
	            		Integer rv = (Integer) iterator2.next();
	            		if (cur == '-')
	            			result = lv - rv;
	            		else if (cur == '+')
	            			result = lv + rv;
	            		else if (cur == '*')
	            			result = lv * rv;
	            		list.add(result);            		
	            	}
	        	}
        	}
        }
        return list;
    }
}