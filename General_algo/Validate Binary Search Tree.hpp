/*Given a binary tree, determine if it is a valid binary search tree (BST).

Assume a BST is defined as follows:

The left subtree of a node contains only nodes with keys less than the node's key.
The right subtree of a node contains only nodes with keys greater than the node's key.
Both the left and right subtrees must also be binary search trees.
Example 1:
    2
   / \
  1   3
Binary tree [2,1,3], return true.
Example 2:
    1
   / \
  2   3
Binary tree [1,2,3], return false.
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
	bool traverse(TreeNode *root, vector<int> &arr)
	{
		if (!root) return true;

		if (root->left)
			if (!traverse(root->left, arr)) return false;

		if (arr.size() && arr[arr.size() - 1] >= root->val) return false;
		arr.push_back(root->val);

		if (root->right) return traverse(root->right, arr);

		return true;
	}
	bool isValidBST(TreeNode* root) {
		if (!root) return true;
		
		vector<int> arr;
		return traverse(root, arr);
	}
};