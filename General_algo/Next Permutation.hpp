/*
Implement next permutation, which rearranges numbers into the lexicographically next greater permutation of numbers.

If such arrangement is not possible, it must rearrange it as the lowest possible order (ie, sorted in ascending order).

The replacement must be in-place, do not allocate extra memory.

Here are some examples. Inputs are in the left-hand column and its corresponding outputs are in the right-hand column.
1,2,3 → 1,3,2
3,2,1 → 1,2,3
1,1,5 → 1,5,1
*/

class Solution {
public:
    void nextPermutation(vector<int>& a) {
        
		if (!a.size())
			return;

		int len = a.size();
		int i = len - 1;
		while (i > 0)
		{
			if (a[i] > a[i - 1])
			{
				for (int j = len - 1; j >= i; j--)
				{
					if (a[j] > a[i - 1])
					{
						swap(a[j], a[i - 1]);
						break;
					}
				}
				break;
			}
			i--;
		}

		for (int j = i; j < (len + i) / 2; j++)
		{
			swap(a[j], a[len - (j - i) - 1]);
		}
    }
};