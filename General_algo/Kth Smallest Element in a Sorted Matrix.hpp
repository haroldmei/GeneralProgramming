/*Given a n x n matrix where each of the rows and columns are sorted in ascending order, find the kth smallest element in the matrix.

Note that it is the kth smallest element in the sorted order, not the kth distinct element.

Example:

matrix = [
   [ 1,  5,  9],
   [10, 11, 13],
   [12, 13, 15]
],
k = 8,

return 13.
*/

int kthSmallest(int** matrix, int n, int useless, int k) {
//It seems strange that I need ColSize for a n*n matrix...
    int small=matrix[0][0], big=matrix[n-1][n-1], mid, cnt, i, j;
    while(small<big) {
        mid=small+big>>1;
        cnt=0;
        j=n-1;
        for(i=0;i<n;i++) {
            while(j>=0&&matrix[i][j]>mid) --j;
            cnt+=j+1;
        }
        if(cnt<k) small=mid+1;
        else big=mid;
    }
    return big;
}


/* An earlier solution that is much much slower

class Solution {
public:
    int kthSmallest(vector<vector<int>>& matrix, int k) {

        vector<int> heap;
        make_heap(heap.begin(), heap.end());
        int n = matrix.size();
        for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            heap.push_back(matrix[i][j]);
            push_heap(heap.begin(), heap.end());
        }

        int count = n * n - k;
        for (int i = 0; i < count; i++)
        {
            std::pop_heap(heap.begin(), heap.end());
            heap.pop_back();
        }

        return heap.front();
    }
};
*/