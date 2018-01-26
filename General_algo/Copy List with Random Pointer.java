/*A linked list is given such that each node contains an additional random pointer which could point to any node in the list or null.

Return a deep copy of the list.
*/


/**
 * A linked list is given such that each node contains an additional random pointer which could point to any node in the list or null.
Return a deep copy of the list.

 * Definition for singly-linked list with a random pointer.
 * class RandomListNode {
 *     int label;
 *     RandomListNode next, random;
 *     RandomListNode(int x) { this.label = x; }
 * };
 */
public class Solution {
    public RandomListNode copyRandomList(RandomListNode head) {
        if (head == null)
            return null;
            
        RandomListNode prev = new RandomListNode(head.label);
        RandomListNode result = prev;
        prev.random = head.random;
        RandomListNode cur = head.next;
        
        while (cur != null)
        {
            prev.next = new RandomListNode(cur.label);
            prev.next.random = cur.random;
            prev = prev.next;
            cur = cur.next;
        }
            
        for (cur = result; cur != null; cur = cur.next)
        {
            if (cur.random == null)
                continue;
                
            RandomListNode random = result;
            for (prev = head; prev != null; prev = prev.next)
            {
                if (cur.random == prev)
                {
                    cur.random = random;
                    break;
                }
                
                random = random.next;
            }
            
        }
            
        return result;
    }
}


/**Another solution that is a bit faster */
public class Solution {
    public RandomListNode copyRandomList(RandomListNode head) {
        if (head == null)
            return null;
            
        RandomListNode prev = new RandomListNode(head.label);
        RandomListNode result = prev;
        prev.random = head.random;
        RandomListNode cur = head.next;
        
        while (cur != null)
        {
            prev.next = new RandomListNode(cur.label);
            prev.next.random = cur.random;
            prev = prev.next;
            cur = cur.next;
        }
            
        for (cur = result; cur != null; cur = cur.next)
        {
                    if (cur == null)
                        System.out.println(cur);
            RandomListNode random = result;
            for (prev = head; prev != null; prev = prev.next)
            {
                if (cur.random == prev)
                {
                    cur.random = random;
                    break;
                }
                
                random = random.next;
            }
            
        }
            
        return result;
    }
}