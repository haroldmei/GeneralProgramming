/*Follow up for H-Index: What if the citations array is sorted in ascending order? Could you optimize your algorithm?
*/

public class Solution {
    public int hIndex(int[] citations) {
        if (citations == null || citations.length == 0)
            return 0;
        
        //Arrays.sort(citations);
        int c = citations.length;
        for (int a : citations)
        {
            if (a >= c)
                return c;
            c--;
        }
        return c;
    }
}