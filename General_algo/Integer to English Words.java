/*Convert a non-negative integer to its english words representation. Given input is guaranteed to be less than 231 - 1.

For example,
123 -> "One Hundred Twenty Three"
12345 -> "Twelve Thousand Three Hundred Forty Five"
1234567 -> "One Million Two Hundred Thirty Four Thousand Five Hundred Sixty Seven"
*/


public class Solution {
	private String oneDigit(int i)
	{
		String result = "";
		switch (i)
		{
		case 0:
			break;
		case 1:
			result = "One";
			break;
		case 2:
			result = "Two";
			break;
		case 3:
			result = "Three";
			break;
		case 4:
			result = "Four";
			break;
		case 5:
			result = "Five";
			break;
		case 6:
			result = "Six";
			break;
		case 7:
			result = "Seven";
			break;
		case 8:
			result = "Eight";
			break;
		case 9:
			result = "Nine";
			break;
		default:
			break;
		}

		return result;
	}
	
	private String hundredToWords(int num)
	{
		int hundred = num/100;
		String result = "";
		if (hundred > 0)
			result = result + oneDigit(hundred) + " Hundred";
			
		int ten = num%100;
		if (ten < 20 && ten >= 10)
		{
		if (result.length() > 0 && ten > 0)
		    result += " ";
			switch (ten)
			{
			case 10:
				result += "Ten";
				break;
			case 11:
				result += "Eleven";
				break;
			case 12:
				result += "Twelve";
				break;
			case 13:
				result += "Thirteen";
				break;
			case 15:
				result += "Fifteen";
				break;
			case 18:
				result += "Eighteen";
				break;
			default:
				result += (oneDigit(ten%10) + "teen");
				break;
			}
			
			return result;
		}
		else if (ten >= 20)
		{
		if (result.length() > 0 && ten > 0)
		    result += " ";
			int tenDigit = ten / 10;
			
			switch (tenDigit)
			{
			case 2:
				result += "Twenty";
				break;
			case 3:
				result += "Thirty";
				break;
			case 4:
				result += "Forty";
				break;
			case 5:
				result += "Fifty";
				break;
			case 8:
				result += "Eighty";
				break;
			default:
				result += (oneDigit(tenDigit) + "ty");
				break;
			}
		}

		int one = ten % 10;
		if (one > 0 && result.length() > 0)
		    result += " ";
		result += (oneDigit(one));
		return result;
	}
    public String numberToWords(int num) {
		if (num == 0)
			return "Zero";
		
		int billion = num/1000000000;
		int million = (num % 1000000000 )/1000000;
        int thousand = ((num % 1000000000 )%1000000)/1000;
		int hundred = (((num % 1000000000 )%1000000)) % 1000;

		String result = "";
		if (billion > 0)
			result = result + (hundredToWords(billion) + " Billion");
		
		if (million > 0 && result.length() > 0)
		    result += " ";
		if (million > 0)
			result = result + hundredToWords(million) + " Million";
		
		if (thousand > 0 && result.length() > 0)
		    result += " ";
		if (thousand > 0)
			result = result + hundredToWords(thousand) + " Thousand";
		
		if (hundred > 0 && result.length() > 0)
		    result += " ";
		if (hundred > 0)
			result = result + hundredToWords(hundred);
		return result;
    }
}