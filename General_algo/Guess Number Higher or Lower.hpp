/*We are playing the Guess Game. The game is as follows:

I pick a number from 1 to n. You have to guess which number I picked.

Every time you guess wrong, I'll tell you whether the number is higher or lower.

You call a pre-defined API guess(int num) which returns 3 possible results (-1, 1, or 0):

-1 : My number is lower
 1 : My number is higher
 0 : Congrats! You got it!
 */

// Forward declaration of guess API.
// @param num, your guess
// @return -1 if my number is lower, 1 if my number is higher, otherwise return 0
int guess(int num);

class Solution {
public:
	int guessNumber(int n) {
		if (n == 1)
			return 1;

		int b = 1;
		int e = n;

		while (1)
		{
			int mid = b + (e - b) / 2;
			int res = guess(mid);
			if (res < 0)
			{
				e = mid - 1;
			}
			else if (res > 0)
			{
				b = mid + 1;
			}
			else
			{
				return mid;
			}
		}
	}
};