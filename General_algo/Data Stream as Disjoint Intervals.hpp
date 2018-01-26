/*Given a data stream input of non-negative integers a1, a2, ..., an, ..., summarize the numbers seen so far as a list of disjoint intervals.

For example, suppose the integers from the data stream are 1, 3, 7, 2, 6, ..., then the summary will be:

[1, 1]
[1, 1], [3, 3]
[1, 1], [3, 3], [7, 7]
[1, 3], [7, 7]
[1, 3], [6, 7]
Follow up:
What if there are lots of merges and the number of disjoint intervals are small compared to the data stream's size?
*/

/**
 * Definition for an interval.
 * struct Interval {
 *     int start;
 *     int end;
 *     Interval() : start(0), end(0) {}
 *     Interval(int s, int e) : start(s), end(e) {}
 * };
 */
class SummaryRanges {
public:
    /** Initialize your data structure here. */
    SummaryRanges() {

    }

    void addNum(int val) {
        if (intervals.size() == 0){
            intervals.push_back(Interval(val, val));
            return;
        }

        vector<Interval>::iterator it = lower_bound(intervals.begin(), intervals.end(), Interval(val, val), [](const Interval &a, const Interval &b){ return a.start < b.start; });
        if (it != intervals.end()){
            if (it == intervals.begin()){
                if (val + 1 >= it->start)
                    it->start = min(val, it->start);
                else
                    intervals.insert(it, Interval(val, val));
            }
            else{
                if (val + 1 >= it->start)
                    it->start = min(val, it->start);
                else
                    it = intervals.insert(it, Interval(val, val));

                vector<Interval>::iterator it1 = it - 1;
                if (val - 1 <= it1->end){
                    it1->end = max(val, it1->end);
                    if (it1->end + 1 >= it->start){
                        it1->end = max(it->end, it1->end);
                        intervals.erase(it);
                    }
                }
            }
        }
        else{
            vector<Interval>::iterator it1 = it - 1;
            if (val - 1 <= it1->end)
                it1->end = max(val, it1->end);
            else
                intervals.push_back(Interval(val, val));
        }
    }

    vector<Interval> getIntervals() {
        return intervals;
    }

    vector<Interval> intervals;
};

/**
 * Your SummaryRanges object will be instantiated and called as such:
 * SummaryRanges obj = new SummaryRanges();
 * obj.addNum(val);
 * vector<Interval> param_2 = obj.getIntervals();
 */