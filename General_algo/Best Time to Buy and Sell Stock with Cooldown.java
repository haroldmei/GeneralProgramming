/*Say you have an array for which the ith element is the price of a given stock on day i.

Design an algorithm to find the maximum profit. You may complete as many transactions as you like (ie, buy one and sell one share of the stock multiple times) with the following restrictions:

You may not engage in multiple transactions at the same time (ie, you must sell the stock before you buy again).
After you sell your stock, you cannot buy stock on next day. (ie, cooldown 1 day)
Example:

prices = [1, 2, 3, 0, 2]
maxProfit = 3
transactions = [buy, sell, cooldown, buy, sell]
*/

public class Solution {
    public int maxProfit(int[] prices) {
        if (prices.length <= 1)
            return 0;
        int[] buys = new int[prices.length];
        int[] sells = new int[prices.length];
        
        sells[0] = 0; 
        buys[0] = -prices[0];
        int max = sells[0];
        for (int i = 1; i < prices.length; i++)
        {
        	int delta = prices[i] - prices[i - 1];
        	sells[i] = Math.max(buys[i - 1] + prices[i], sells[i - 1] + delta);
        	buys[i] = i > 1 ? Math.max(sells[i - 2] - prices[i], buys[i - 1] - delta) : buys[i - 1] - delta;
        	max = max < sells[i] ? sells[i] : max;
        }

        
        return max;
    }
}