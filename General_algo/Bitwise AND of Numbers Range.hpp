/*Given a range [m, n] where 0 <= m <= n <= 2147483647, return the bitwise AND of all numbers in this range, inclusive.

For example, given the range [5, 7], you should return 4.
*/Bitwise AND of Numbers Range

class Solution {
public:
    int rangeBitwiseAnd(int m, int n) {
		if (m == 0 || n == 0) return 0;

		vector<unsigned int> sec = {1, 2, 4, 8, 16, 32, 64, 128,
					256, 512, 1024, 2048, 4096, 8192, 16384, 32768,
					65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608 ,
					16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648 };

		vector<unsigned int>::iterator mi = find_if(sec.begin(), sec.end(), [m](unsigned int s) { return s > m; });
		vector<unsigned int>::iterator ni = find_if(sec.begin(), sec.end(), [n](unsigned int s) { return s > n; });
		if (mi != ni)
			return 0;
		if (mi == sec.begin())
			return 1;

		mi--; ni--;
		return *mi | rangeBitwiseAnd(m - *mi, n - *ni);
	}
};