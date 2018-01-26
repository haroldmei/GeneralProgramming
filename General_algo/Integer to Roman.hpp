/*Given an integer, convert it to a roman numeral.

Input is guaranteed to be within the range from 1 to 3999.
*/

class Solution {
public:
    string intToRoman(int num){
		string str;

		int thousand = num / 1000;
		int handred = (num % 1000) / 100;
		int ten = (num % 100) / 10;
		int one = num % 10;

		str.append(thousand, 'M');

		if (handred <= 3)
			str.append(handred, 'C');
		else if (handred <= 5){
			str.append(5 - handred, 'C');
			str.append(1, 'D');
		}
		else if (handred <= 8){
			str.append(1, 'D');
			str.append(handred - 5, 'C');
		}
		else{
			str.append(10 - handred, 'C');
			str.append(1, 'M');
		}

		if (ten <= 3)
			str.append(ten, 'X');
		else if (ten <= 5){
			str.append(5 - ten, 'X');
			str.append(1, 'L');
		}
		else if (ten <= 8){
			str.append(1, 'L');
			str.append(ten - 5, 'X');
		}
		else{
			str.append(10 - ten, 'X');
			str.append(1, 'C');
		}

		if (one <= 3)
			str.append(one, 'I');
		else if (one <= 5){
			str.append(5 - one, 'I');
			str.append(1, 'V');
		}
		else if (one <= 8){
			str.append(1, 'V');
			str.append(one - 5, 'I');
		}
		else{
			str.append(10 - one, 'I');
			str.append(1, 'X');
		}

		return str;
	}
};