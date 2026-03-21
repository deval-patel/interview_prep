/*
 * Q21 — Async Network Message Router [Medium]
 *
 * Topic: Parallel Programming — Networking
 *
 * Problem:
 *   Simulate an asynchronous message-passing network between nodes.
 *   Each node has an inbox. Messages travel between nodes with per-link
 *   latencies defined in a latency matrix.
 *
 *   Implement:
 *   1. Network(num_nodes, latency_matrix) — create network with per-link
 *      latencies. latency_matrix[i][j] = latency from node i to node j.
 *      A value of 0 means no direct link (unreachable directly).
 *   2. send(src, dst, message, timestamp) — enqueue a message from src to dst
 *      at the given timestamp. It arrives at timestamp + latency_matrix[src][dst].
 *   3. step() — advance simulation by one time unit, deliver messages whose
 *      latency has elapsed, return list of delivered messages.
 *   4. broadcast(src, message, timestamp) — send message to all other nodes.
 *   5. shortest_path_latency(src, dst) — compute the minimum-latency path
 *      from src to dst using Dijkstra's algorithm. Return -1 if unreachable.
 *
 * Constraints:
 *   - Node IDs are 0 to num_nodes - 1
 *   - Latencies are positive integers (0 = no direct link)
 *   - Messages are delivered in order of arrival time; ties broken by
 *     send order (FIFO)
 *   - Latency matrix may be asymmetric (latency[i][j] != latency[j][i])
 *
 * Key Concepts:
 *   - Discrete event simulation
 *   - Dijkstra's shortest path algorithm
 *   - Message-passing models (CSP, actor model)
 *   - Network latency and routing
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <climits>

using namespace std;

struct Message {
    int src;
    int dst;
    string content;
    int send_time;
    int arrive_time;
};

// ============ YOUR IMPLEMENTATION ============

struct Network {
    int num_nodes;
    int current_time;

    // TODO: Add data structures for:
    //   - Latency matrix (num_nodes x num_nodes)
    //   - In-flight messages (priority queue or sorted list)
    //   - Per-node inboxes
    //   - Message send counter (for FIFO tie-breaking)

    Network(int num_nodes, const vector<vector<int>>& latency_matrix)
        : num_nodes(num_nodes), current_time(0) {
        // TODO: Store latency matrix
        // TODO: Initialize per-node inboxes and message queue
    }

    /*
     * send: Enqueue a message from src to dst.
     * The message arrives at timestamp + latency_matrix[src][dst].
     * If there is no direct link (latency == 0), the send is ignored.
     */
    void send(int src, int dst, const string& message, int timestamp) {
        // TODO: Create Message with arrive_time = timestamp + latency[src][dst]
        //   Add to in-flight queue
        //   Update current_time if timestamp > current_time
    }

    /*
     * broadcast: Send message from src to all other nodes at given timestamp.
     */
    void broadcast(int src, const string& message, int timestamp) {
        // TODO: For each node != src, call send(src, node, message, timestamp)
    }

    /*
     * step: Advance simulation by one time unit.
     * Deliver all in-flight messages whose arrive_time <= current_time.
     * Delivered messages are added to the destination node's inbox.
     * Return list of messages delivered this step.
     */
    vector<Message> step() {
        vector<Message> delivered;

        // TODO: current_time++
        //   For each in-flight message with arrive_time <= current_time:
        //     - Add to dst node's inbox
        //     - Add to delivered list
        //     - Remove from in-flight queue
        //   Return delivered (in FIFO order)

        return delivered;
    }

    /*
     * get_inbox: Return all messages received by a node so far.
     */
    vector<Message> get_inbox(int node) {
        // TODO: Return the inbox for the given node
        return {};
    }

    /*
     * shortest_path_latency: Compute minimum latency from src to dst
     * using Dijkstra's algorithm on the latency matrix.
     * Only edges with latency > 0 exist.
     * Return -1 if dst is unreachable from src.
     */
    int shortest_path_latency(int src, int dst) {
        // TODO: Dijkstra's algorithm
        //   dist[src] = 0, all others = INT_MAX
        //   Priority queue of (distance, node)
        //   For each neighbor with latency > 0, relax edges
        //   Return dist[dst] or -1 if unreachable
        return -1;
    }
};

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q21: Async Network Message Router ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Direct send arrives after correct latency
    {
        total++;
        vector<vector<int>> lat = {
            {0, 3, 0},
            {3, 0, 2},
            {0, 2, 0}
        };
        Network net(3, lat);
        net.send(0, 1, "hello", 0);

        // Step 3 times; message should arrive at time 3
        vector<Message> delivered;
        for (int t = 0; t < 3; t++) {
            auto d = net.step();
            delivered.insert(delivered.end(), d.begin(), d.end());
        }

        bool ok = (delivered.size() == 1 &&
                   delivered[0].content == "hello" &&
                   delivered[0].dst == 1 &&
                   delivered[0].arrive_time == 3);

        printf("%s [Direct send: delivered %zu msg(s) at time 3, content='%s']\n",
               ok ? "PASS" : "FAIL", delivered.size(),
               delivered.empty() ? "" : delivered[0].content.c_str());
        if (ok) passed++;
    }

    // Test 2: Broadcast reaches all nodes
    {
        total++;
        vector<vector<int>> lat = {
            {0, 1, 2},
            {1, 0, 1},
            {2, 1, 0}
        };
        Network net(3, lat);
        net.broadcast(0, "bcast", 0);

        // Step enough times for all to arrive
        for (int t = 0; t < 5; t++) net.step();

        auto inbox1 = net.get_inbox(1);
        auto inbox2 = net.get_inbox(2);
        auto inbox0 = net.get_inbox(0);

        bool ok = (inbox1.size() == 1 && inbox2.size() == 1 && inbox0.size() == 0 &&
                   inbox1[0].content == "bcast" && inbox2[0].content == "bcast");

        printf("%s [Broadcast: node1 got %zu, node2 got %zu, node0 got %zu (exp 1,1,0)]\n",
               ok ? "PASS" : "FAIL", inbox1.size(), inbox2.size(), inbox0.size());
        if (ok) passed++;
    }

    // Test 3: Messages delivered in send order (FIFO)
    {
        total++;
        vector<vector<int>> lat = {
            {0, 2},
            {2, 0}
        };
        Network net(2, lat);
        net.send(0, 1, "first", 0);
        net.send(0, 1, "second", 0);

        for (int t = 0; t < 3; t++) net.step();

        auto inbox = net.get_inbox(1);
        bool ok = (inbox.size() == 2 &&
                   inbox[0].content == "first" &&
                   inbox[1].content == "second");

        printf("%s [FIFO order: got '%s' then '%s' (exp 'first' then 'second')]\n",
               ok ? "PASS" : "FAIL",
               inbox.size() > 0 ? inbox[0].content.c_str() : "?",
               inbox.size() > 1 ? inbox[1].content.c_str() : "?");
        if (ok) passed++;
    }

    // Test 4: Shortest path multi-hop
    {
        total++;
        // Node 0 -> Node 2 directly costs 10
        // Node 0 -> Node 1 -> Node 2 costs 3 + 2 = 5
        vector<vector<int>> lat = {
            {0, 3, 10},
            {3, 0, 2},
            {10, 2, 0}
        };
        Network net(3, lat);
        int sp = net.shortest_path_latency(0, 2);
        bool ok = (sp == 5);
        printf("%s [Shortest path 0->2: %d (exp 5, via node 1)]\n",
               ok ? "PASS" : "FAIL", sp);
        if (ok) passed++;
    }

    // Test 5: Asymmetric latencies
    {
        total++;
        vector<vector<int>> lat = {
            {0, 1, 0},
            {5, 0, 1},
            {0, 1, 0}
        };
        Network net(3, lat);

        int forward = net.shortest_path_latency(0, 2);   // 0->1(1) + 1->2(1) = 2
        int backward = net.shortest_path_latency(2, 0);   // 2->1(1) + 1->0(5) = 6

        bool ok = (forward == 2 && backward == 6);
        printf("%s [Asymmetric: 0->2=%d (exp 2), 2->0=%d (exp 6)]\n",
               ok ? "PASS" : "FAIL", forward, backward);
        if (ok) passed++;
    }

    // Test 6: Multiple in-flight messages with different arrival times
    {
        total++;
        vector<vector<int>> lat = {
            {0, 1, 3},
            {1, 0, 1},
            {3, 1, 0}
        };
        Network net(3, lat);
        net.send(0, 1, "fast", 0);   // arrives at 1
        net.send(0, 2, "slow", 0);   // arrives at 3
        net.send(1, 2, "mid", 1);    // arrives at 1 + 1 = 2

        vector<int> delivery_times;
        for (int t = 0; t < 5; t++) {
            auto d = net.step();
            for (auto& m : d) delivery_times.push_back(m.arrive_time);
        }

        // Should deliver at times 1, 2, 3
        sort(delivery_times.begin(), delivery_times.end());
        bool ok = (delivery_times.size() == 3 &&
                   delivery_times[0] == 1 &&
                   delivery_times[1] == 2 &&
                   delivery_times[2] == 3);

        printf("%s [Multi-flight: deliveries at times ", ok ? "PASS" : "FAIL");
        for (size_t i = 0; i < delivery_times.size(); i++) {
            printf("%d%s", delivery_times[i], i + 1 < delivery_times.size() ? "," : "");
        }
        printf(" (exp 1,2,3)]\n");
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
