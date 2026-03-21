"""
Q21 — Async Network Message Router [Medium]

Topic: Parallel Programming — Networking

Problem:
  Simulate an asynchronous message-passing network between nodes.
  Each node has an inbox. Messages travel between nodes with per-link
  latencies defined in a latency matrix.

  Implement:
  1. Network(num_nodes, latency_matrix) — create network with per-link
     latencies. latency_matrix[i][j] = latency from node i to node j.
     A value of 0 means no direct link (unreachable directly).
  2. send(src, dst, message, timestamp) — enqueue a message from src to dst
     at the given timestamp. It arrives at timestamp + latency_matrix[src][dst].
  3. step() — advance simulation by one time unit, deliver messages whose
     latency has elapsed, return list of delivered messages.
  4. broadcast(src, message, timestamp) — send message to all other nodes.
  5. shortest_path_latency(src, dst) — compute the minimum-latency path
     from src to dst using Dijkstra's algorithm. Return -1 if unreachable.

Constraints:
  - Node IDs are 0 to num_nodes - 1
  - Latencies are positive integers (0 = no direct link)
  - Messages are delivered in order of arrival time; ties broken by
    send order (FIFO)
  - Latency matrix may be asymmetric (latency[i][j] != latency[j][i])

Key Concepts:
  - Discrete event simulation
  - Dijkstra's shortest path algorithm
  - Message-passing models (CSP, actor model)
  - Network latency and routing
"""

import heapq


class Message:
    def __init__(self, src, dst, content, send_time, arrive_time):
        self.src = src
        self.dst = dst
        self.content = content
        self.send_time = send_time
        self.arrive_time = arrive_time


class Network:
    def __init__(self, num_nodes, latency_matrix):
        self.num_nodes = num_nodes
        self.current_time = 0
        # TODO: Store latency matrix
        # TODO: Initialize per-node inboxes (list of lists)
        # TODO: Initialize in-flight message queue
        # TODO: Initialize send counter for FIFO ordering

    def send(self, src, dst, message, timestamp):
        """
        Enqueue a message from src to dst.
        Arrives at timestamp + latency_matrix[src][dst].
        If no direct link (latency == 0), the send is ignored.
        """
        # TODO: Create Message with correct arrive_time
        #   Add to in-flight queue
        #   Update current_time if needed
        pass

    def broadcast(self, src, message, timestamp):
        """Send message from src to all other nodes."""
        # TODO: For each node != src, call self.send(...)
        pass

    def step(self):
        """
        Advance simulation by one time unit.
        Deliver all in-flight messages whose arrive_time <= current_time.
        Return list of delivered Messages.
        """
        # TODO: self.current_time += 1
        #   Collect and deliver messages with arrive_time <= current_time
        #   Add to destination inbox, return delivered list
        return []

    def get_inbox(self, node):
        """Return all messages received by a node so far."""
        # TODO: Return the inbox list for the given node
        return []

    def shortest_path_latency(self, src, dst):
        """
        Compute minimum latency from src to dst using Dijkstra.
        Only edges with latency > 0 exist. Return -1 if unreachable.
        """
        # TODO: Dijkstra's algorithm
        #   dist = [inf] * num_nodes; dist[src] = 0
        #   pq = [(0, src)]
        #   While pq not empty:
        #     pop (d, u), skip if d > dist[u]
        #     for each neighbor v with latency[u][v] > 0:
        #       if dist[u] + latency[u][v] < dist[v]: relax
        #   Return dist[dst] if reachable, else -1
        return -1


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q21: Async Network Message Router ===\n")
    passed = 0
    total = 0

    # Test 1: Direct send arrives after correct latency
    total += 1
    lat = [
        [0, 3, 0],
        [3, 0, 2],
        [0, 2, 0],
    ]
    net = Network(3, lat)
    net.send(0, 1, "hello", 0)

    delivered = []
    for _ in range(3):
        delivered.extend(net.step())

    ok = (len(delivered) == 1 and
          delivered[0].content == "hello" and
          delivered[0].dst == 1 and
          delivered[0].arrive_time == 3)
    print(f"{'PASS' if ok else 'FAIL'} [Direct send: delivered {len(delivered)} msg(s) "
          f"at time 3, content='{delivered[0].content if delivered else '?'}']")
    if ok:
        passed += 1

    # Test 2: Broadcast reaches all nodes
    total += 1
    lat = [
        [0, 1, 2],
        [1, 0, 1],
        [2, 1, 0],
    ]
    net = Network(3, lat)
    net.broadcast(0, "bcast", 0)
    for _ in range(5):
        net.step()

    inbox1 = net.get_inbox(1)
    inbox2 = net.get_inbox(2)
    inbox0 = net.get_inbox(0)
    ok = (len(inbox1) == 1 and len(inbox2) == 1 and len(inbox0) == 0 and
          inbox1[0].content == "bcast" and inbox2[0].content == "bcast")
    print(f"{'PASS' if ok else 'FAIL'} [Broadcast: node1 got {len(inbox1)}, "
          f"node2 got {len(inbox2)}, node0 got {len(inbox0)} (exp 1,1,0)]")
    if ok:
        passed += 1

    # Test 3: Messages delivered in send order (FIFO)
    total += 1
    lat = [
        [0, 2],
        [2, 0],
    ]
    net = Network(2, lat)
    net.send(0, 1, "first", 0)
    net.send(0, 1, "second", 0)
    for _ in range(3):
        net.step()

    inbox = net.get_inbox(1)
    ok = (len(inbox) == 2 and
          inbox[0].content == "first" and
          inbox[1].content == "second")
    print(f"{'PASS' if ok else 'FAIL'} [FIFO order: got "
          f"'{inbox[0].content if len(inbox) > 0 else '?'}' then "
          f"'{inbox[1].content if len(inbox) > 1 else '?'}' (exp 'first' then 'second')]")
    if ok:
        passed += 1

    # Test 4: Shortest path multi-hop
    total += 1
    lat = [
        [0, 3, 10],
        [3, 0, 2],
        [10, 2, 0],
    ]
    net = Network(3, lat)
    sp = net.shortest_path_latency(0, 2)
    ok = sp == 5
    print(f"{'PASS' if ok else 'FAIL'} [Shortest path 0->2: {sp} (exp 5, via node 1)]")
    if ok:
        passed += 1

    # Test 5: Asymmetric latencies
    total += 1
    lat = [
        [0, 1, 0],
        [5, 0, 1],
        [0, 1, 0],
    ]
    net = Network(3, lat)
    forward = net.shortest_path_latency(0, 2)   # 0->1(1) + 1->2(1) = 2
    backward = net.shortest_path_latency(2, 0)  # 2->1(1) + 1->0(5) = 6
    ok = forward == 2 and backward == 6
    print(f"{'PASS' if ok else 'FAIL'} [Asymmetric: 0->2={forward} (exp 2), "
          f"2->0={backward} (exp 6)]")
    if ok:
        passed += 1

    # Test 6: Multiple in-flight messages with different arrival times
    total += 1
    lat = [
        [0, 1, 3],
        [1, 0, 1],
        [3, 1, 0],
    ]
    net = Network(3, lat)
    net.send(0, 1, "fast", 0)   # arrives at 1
    net.send(0, 2, "slow", 0)   # arrives at 3
    net.send(1, 2, "mid", 1)    # arrives at 2

    delivery_times = []
    for _ in range(5):
        d = net.step()
        for m in d:
            delivery_times.append(m.arrive_time)

    delivery_times.sort()
    ok = delivery_times == [1, 2, 3]
    print(f"{'PASS' if ok else 'FAIL'} [Multi-flight: deliveries at times "
          f"{delivery_times} (exp [1, 2, 3])]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
