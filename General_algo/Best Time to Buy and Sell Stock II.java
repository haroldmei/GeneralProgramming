/*Say you have an array for which the ith element is the price of a given stock on day i.

Design an algorithm to find the maximum profit. You may complete as many transactions as you like (ie, buy one and sell one share of the stock multiple times). However, you may not engage in multiple transactions at the same time (ie, you must sell the stock before you buy again).

*/

public class Solution {
	public static int maxProfit(int[] prices)
	{
		if (prices.length == 1)
			return 0;

		if (prices.length == 2)
			return prices[1] > prices[0] ? prices[1] - prices[0] : 0;
			
		int b = prices.length/2 - 1;
		int e = prices.length/2 + 1;
		int max = 0;

		while(b >= 0 && e < prices.length)
		{
			int max1 = prices[b + 1] > prices[b] ? prices[b + 1] - prices[b] : 0;
			int max2 = prices[e] > prices[e - 1] ? prices[e] - prices[e - 1] : 0;
			max += max1 + max2;
			System.out.println(b + " " + e + " " + max1 + " " + max2 + " " + max);

			b--;
			e++;
		}
		if (e == prices.length && b == 0)
			max += prices[1] > prices[0] ? prices[1] - prices[0] : 0;
		
		return max;
	}
}