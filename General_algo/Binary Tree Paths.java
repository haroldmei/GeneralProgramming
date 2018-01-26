/*Given a binary tree, return all root-to-leaf paths.

For example, given the following binary tree:

   1
 /   \
2     3
 \
  5
All root-to-leaf paths are:

["1->2->5", "1->3"]
*/

/**
 * Definition for a binary tree node.
 * public class TreeNode {
 *     int val;
 *     TreeNode left;
 *     TreeNode right;
 *     TreeNode(int x) { val = x; }
 * }
 */
public class Solution {
    public List<String> binaryTreePaths(TreeNode root) {
    	List<String> list = new ArrayList<String>();
    	if (root == null)
    		return list;

    	String s = "" + root.val;
        if (root.left == null && root.right == null)
        {
        	list.add(s);
        	return list;
        }
        
        List <String> list1 = binaryTreePaths(root.left);
        for (int i = 0; i < list1.size(); i++)
        {
        	String cur = list1.get(i);
        	cur = s + "->" + cur;
        	list1.set(i, cur);
        }
        	
        List <String> list2 = binaryTreePaths(root.right);
        for (int i = 0; i < list2.size(); i++)
        {
        	String cur = list2.get(i);
        	cur = s + "->" + cur;
        	list2.set(i, cur);
        }
        list1.addAll(list2);
        return list1;
        
    }
}