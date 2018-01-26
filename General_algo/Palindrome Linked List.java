/*Given a singly linked list, determine if it is a palindrome.

Follow up:
Could you do it in O(n) time and O(1) space?
*/

/**
 * Definition for singly-linked list.
 * public class ListNode {
 *     int val;
 *     ListNode next;
 *     ListNode(int x) { val = x; }
 * }
 */
public class Solution {
    public boolean isPalindrome(ListNode head) {
        
        if (head == null)
            return true;
        
        ListNode cur = head;
        ListNode mid = head;
        ListNode tail = head;
        int count = 0;
        for (; cur != null; cur = cur.next)count++;
        for (int i = 0; i < count/2; i++)mid = mid.next;

        cur = mid;
        tail = mid;
        while (tail.next != null)
        {
            mid = tail.next;
            tail.next = mid.next;
            mid.next = cur;
            cur = mid;
        }
        
        //tail.next = null;
        for (; mid != null; mid = mid.next, head = head.next)
        {
            if (mid.val != head.val)
                return false;
        }
        
        return true;
    }
}