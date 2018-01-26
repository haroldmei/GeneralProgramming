/*
Shuffle a set of numbers without duplicates.

Example:

// Init an array with set 1, 2, and 3.
int[] nums = {1,2,3};
Solution solution = new Solution(nums);

// Shuffle the array [1,2,3] and return its result. Any permutation of [1,2,3] must equally likely to be returned.
solution.shuffle();

// Resets the array back to its original configuration [1,2,3].
solution.reset();

// Returns the random shuffling of array [1,2,3].
solution.shuffle();
*/


class Solution {
public:
    Solution(vector<int> nums) {
        m_array = nums;
        m_array_shuffle = nums;
    }

    /** Resets the array to its original configuration and return it. */
    vector<int> &reset() {
        m_array_shuffle = m_array;
        return m_array_shuffle;
    }

    /** Returns a random shuffling of the array. */
    vector<int> &shuffle() {

        int len = m_array_shuffle.size();
        while (len){
            int randv = rand();
            int ind = randv % len;
            swap(m_array_shuffle[len - 1], m_array_shuffle[ind]);
            len--;
        }
        return m_array_shuffle;
    }

    vector<int> m_array_shuffle;
    vector<int> m_array;
};

/**
 * Your Solution object will be instantiated and called as such:
 * Solution obj = new Solution(nums);
 * vector<int> param_1 = obj.reset();
 * vector<int> param_2 = obj.shuffle();
 */