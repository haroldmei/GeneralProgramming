/*Given a binary tree, check whether it is a mirror of itself (ie, symmetric around its center).

For example, this binary tree [1,2,2,3,4,4,3] is symmetric:

    1
   / \
  2   2
 / \ / \
3  4 4  3
But the following [1,2,2,null,3,null,3] is not:
    1
   / \
  2   2
   \   \
   3    3
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
    void flip(TreeNode* root)
    {
        if (!root)
            return;
        TreeNode* tmp = root->left;
        root->left = root->right;
        root->right = tmp;
        flip(root->left);
        flip(root->right);
    }
    bool isEqual(TreeNode *t1, TreeNode *t2)
    {
        if (t1 && t2 && t1->val == t2->val)
            return isEqual(t1->left, t2->left) && isEqual(t1->right, t2->right);
        else if (!t1 && !t2)
            return true;
            
        return false;
    }
    bool isSymmetric(TreeNode* root) {
        if (!root)
            return true;
            
        flip(root->right);
        return isEqual(root->left, root->right);
    }
};