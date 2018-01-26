/*Given a complete binary tree, count the number of nodes.

Definition of a complete binary tree from Wikipedia:
In a complete binary tree every level, except possibly the last, is completely filled, and all nodes in the last level are as far left as possible. It can have between 1 and 2h nodes inclusive at the last level h.
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
    bool isFull(TreeNode *head, int &dr){
        if (!head)
            return true;
        int dl = 0;
        for (TreeNode *h = head; h; h = h->left) 
            dl++;

        for (TreeNode *h = head; h; h = h->right) 
            dr++;
        return dl == dr;
    }
    int countNodes(TreeNode* root) {

        int dr = 0;
        if (isFull(root, dr))
            return (1 << dr) - 1;

        else
            return countNodes(root->left) + countNodes(root->right) + 1;
    }
};