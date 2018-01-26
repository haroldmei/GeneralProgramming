/*Given a collection of intervals, merge all overlapping intervals.

For example,
Given [1,3],[2,6],[8,10],[15,18],
return [1,6],[8,10],[15,18].


*/


/**
 * Definition for an interval.
 * public class Interval {
 *     int start;
 *     int end;
 *     Interval() { start = 0; end = 0; }
 *     Interval(int s, int e) { start = s; end = e; }
 * }
 */
public class Solution {
    public List<Interval> merge(List<Interval> intervals) {
        
		if (intervals == null || intervals.size() == 0)
			return intervals;
		
        Collections.sort(intervals, new Comparator<Interval>() {
			public int compare(Interval e1, Interval e2)
			{
				return e1.start > e2.start ? 1 : e1.start < e2.start ? -1 : 0;
			}
		});

		Interval last = intervals.get(0);
		for (int i = 1; i < intervals.size(); )
		{
			Interval curr = intervals.get(i);
			if (last.end < curr.start)
			{
				last = curr;
				i++;
			}
			else
			{
				last.end = Math.max(last.end, curr.end);
				intervals.remove(i);
			}
		}

		return intervals;
    }
}