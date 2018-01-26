/*Given a list of airline tickets represented by pairs of departure and arrival airports [from, to], reconstruct the itinerary in order. All of the tickets belong to a man who departs from JFK. Thus, the itinerary must begin with JFK.

Note:
If there are multiple valid itineraries, you should return the itinerary that has the smallest lexical order when read as a single string. For example, the itinerary ["JFK", "LGA"] has a smaller lexical order than ["JFK", "LGB"].
All airports are represented by three capital letters (IATA code).
You may assume all tickets form at least one valid itinerary.
Example 1:
tickets = [["MUC", "LHR"], ["JFK", "MUC"], ["SFO", "SJC"], ["LHR", "SFO"]]
Return ["JFK", "MUC", "LHR", "SFO", "SJC"].
Example 2:
tickets = [["JFK","SFO"],["JFK","ATL"],["SFO","ATL"],["ATL","JFK"],["ATL","SFO"]]
Return ["JFK","ATL","JFK","SFO","ATL","SFO"].
Another possible reconstruction is ["JFK","SFO","ATL","JFK","ATL","SFO"]. But it is larger in lexical order.
*/

class Solution {
public:

	bool dfsItinerary(const vector<pair<string, string>> &tickets, string from, vector<int> &visited, vector<string> &res) {

		if (res.size() == tickets.size())
			return true;

		int len = tickets.size();
		for (int i = 0; i < len; i++){
			const pair<string, string> &cur = tickets[i];
			if (tickets[i].first == from && !visited[i]){
				string next = tickets[i].second;

				visited[i] = 1;
				res.push_back(next);
				if (!dfsItinerary(tickets, next, visited, res)){
					res.pop_back();
					visited[i] = 0;
				}
				else
					return true;
			}
		}

		return false;
	}

	vector<string> findItinerary(vector<pair<string, string>> tickets) {

		sort(tickets.begin(), tickets.end());
		vector<string> res;
		vector<int> visited(tickets.size(), 0);

		dfsItinerary(tickets, "JFK", visited, res);
		res.insert(res.begin(), "JFK");
		return res;
	}
};