/*Given a binary tree, find its maximum depth.

The maximum depth is the number of nodes along the longest path from the root node down to the farthest leaf node.
*/


/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    int maxDepth(TreeNode* root) {
        if (!root)
            return 0;
            
        int depthLeft = maxDepth(root->left);
        int depthRight = maxDepth(root->right);
        return depthLeft > depthRight ? (depthLeft + 1) : (depthRight + 1);
    }
};