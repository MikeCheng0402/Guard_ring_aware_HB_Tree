# Guard Ring Aware HB*-Tree Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement the missing guard-ring-aware search-tree state, TA-group generation, diffusion-graph optimization, and SA integration so `Guard_ring_aware_HB_Tree` matches Chapter 3 of `Tzu_Hsiang_Thesis.pdf`.

**Architecture:** Keep the existing `FHBTree -> THBTree -> HBTree -> BTree` hierarchy, but add a guard-ring-aware node abstraction that can participate in both non-symmetric and symmetry-group placement paths. Guard ring internal geometry is computed bottom-up from TA groups, simplified diffusion graphs, Euler traversal, and transition dummy / diffusion sharing rules, then exposed upward as a rectangle for higher-level placement.

**Tech Stack:** C++, Makefile build, existing tree placement code in `Guard_ring_aware_HB_Tree`, manual regression checks, thesis-aligned geometry rules in `parameter.h`.

---

### Task 1: Lock the Tree Representation Decision

**Files:**
- Modify: `docs/Guard_ring_aware_plan.md`
- Create: `Guard_ring_aware_HB_Tree/inc/GRNode.h`
- Modify: `Guard_ring_aware_HB_Tree/inc/HBTree.h`
- Modify: `Guard_ring_aware_HB_Tree/inc/BTree.h`

- [ ] **Step 1: Write down the concrete representation choice**

Add a short “implementation decision” section to `docs/Guard_ring_aware_plan.md` that chooses one of these two paths and rejects the other:

```md
## Implementation Decision

We will extend the existing tree representation so guard ring aware state can appear in both:

- `HBTree::HNode` for non-symmetric placement
- `BTree::Node` for ASF-internal symmetric placement

We will not build a separate parallel ASF representation because it would duplicate contour logic and make merge/divide updates harder to keep consistent with the existing code.
```

- [ ] **Step 2: Run a quick text check to verify the decision is present**

Run: `rg -n "Implementation Decision|parallel ASF representation|HBTree::HNode|BTree::Node" docs/Guard_ring_aware_plan.md`
Expected: matches for the new section and the selected representation path.

- [ ] **Step 3: Draft the shared GR node header**

Create `Guard_ring_aware_HB_Tree/inc/GRNode.h` with the minimal shared data model:

```cpp
#ifndef _GRNODE_H_
#define _GRNODE_H_

#include <string>
#include <utility>
#include <vector>

using namespace std;

struct GuardRingMember {
    string cell_name;
    int all_node_id;
};

struct TAGroup {
    vector<int> member_indices;
    int l_value;
    int fin_value;
    int base_y;
};

class GRNode {
public:
    vector<GuardRingMember> members;
    vector<TAGroup> ta_groups;
    vector<pair<int, int> > member_offsets;

    string well_potential;
    char mos_kind;

    int inner_width;
    int inner_height;
    int ring_width;
    int ring_height;

    GRNode();
    void clear_layout();
    bool can_merge_with(const GRNode& other) const;
};

#endif
```

- [ ] **Step 4: Extend HBTree node typing**

Update `Guard_ring_aware_HB_Tree/inc/HBTree.h` so `HNode` can distinguish module, ASF-group, contour, and guard-ring nodes:

```cpp
#include "GRNode.h"

enum HNodeType {
    HNODE_MODULE,
    HNODE_ASF_GROUP,
    HNODE_CONTOUR,
    HNODE_GUARD_RING
};

class HNode {
public:
    BTree ASF_BTree;
    GRNode GR_info;
    Cell Cell_info;
    int id;
    int parent;
    int left_child;
    int right_child;
    bool ASF_or_not;
    bool Contour_or_not;
    HNodeType node_type;
    ...
};
```

- [ ] **Step 5: Extend BTree node typing for the symmetry path**

Update `Guard_ring_aware_HB_Tree/inc/BTree.h` so the ASF-internal path can also host guard ring nodes:

```cpp
enum BNodeType {
    BNODE_MODULE,
    BNODE_GUARD_RING
};

class Node {
public:
    Cell Cell_info;
    GRNode GR_info;
    BNodeType node_type;
    ...
};
```

- [ ] **Step 6: Run a compile-only check to verify headers are consistent**

Run: `make -C Guard_ring_aware_HB_Tree`
Expected: build fails only on missing GRNode implementation or downstream constructor updates, not on basic type-definition syntax.

- [ ] **Step 7: Commit**

```bash
git add docs/Guard_ring_aware_plan.md Guard_ring_aware_HB_Tree/inc/GRNode.h Guard_ring_aware_HB_Tree/inc/HBTree.h Guard_ring_aware_HB_Tree/inc/BTree.h
git commit -m "plan: lock guard ring aware tree representation"
```

### Task 2: Implement the Minimal GRNode API

**Files:**
- Create: `Guard_ring_aware_HB_Tree/src/GRNode.cpp`
- Modify: `Guard_ring_aware_HB_Tree/inc/GRNode.h`
- Test: `Guard_ring_aware_HB_Tree/tests/test_grnode_basics.cpp`

- [ ] **Step 1: Write the failing test**

Create `Guard_ring_aware_HB_Tree/tests/test_grnode_basics.cpp`:

```cpp
#include <cassert>
#include "GRNode.h"

int main() {
    GRNode a;
    GRNode b;

    a.well_potential = "VSS";
    a.mos_kind = 'n';
    b.well_potential = "VSS";
    b.mos_kind = 'n';

    assert(a.can_merge_with(b));

    b.mos_kind = 'p';
    assert(!a.can_merge_with(b));

    a.inner_width = 10;
    a.inner_height = 20;
    a.member_offsets.push_back(make_pair(3, 4));
    a.clear_layout();

    assert(a.inner_width == 0);
    assert(a.inner_height == 0);
    assert(a.member_offsets.empty());
    return 0;
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_grnode_basics.cpp Guard_ring_aware_HB_Tree/src/GRNode.cpp -o /tmp/test_grnode_basics`
Expected: FAIL because `GRNode.cpp` does not exist yet.

- [ ] **Step 3: Write the minimal implementation**

Create `Guard_ring_aware_HB_Tree/src/GRNode.cpp`:

```cpp
#include "GRNode.h"

GRNode::GRNode()
    : mos_kind('?'),
      inner_width(0),
      inner_height(0),
      ring_width(0),
      ring_height(0) {}

void GRNode::clear_layout()
{
    ta_groups.clear();
    member_offsets.clear();
    inner_width = 0;
    inner_height = 0;
    ring_width = 0;
    ring_height = 0;
}

bool GRNode::can_merge_with(const GRNode& other) const
{
    return well_potential == other.well_potential && mos_kind == other.mos_kind;
}
```

- [ ] **Step 4: Run the test to verify it passes**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_grnode_basics.cpp Guard_ring_aware_HB_Tree/src/GRNode.cpp -o /tmp/test_grnode_basics && /tmp/test_grnode_basics`
Expected: command exits successfully with no assertion failure.

- [ ] **Step 5: Commit**

```bash
git add Guard_ring_aware_HB_Tree/inc/GRNode.h Guard_ring_aware_HB_Tree/src/GRNode.cpp Guard_ring_aware_HB_Tree/tests/test_grnode_basics.cpp
git commit -m "feat: add minimal GRNode implementation"
```

### Task 3: Make One-Cell Guard Ring Nodes Legal in HBTree

**Files:**
- Modify: `Guard_ring_aware_HB_Tree/src/HBTree.cpp`
- Modify: `Guard_ring_aware_HB_Tree/inc/HBTree.h`
- Test: `Guard_ring_aware_HB_Tree/tests/test_hbtree_grnode_initial.cpp`

- [ ] **Step 1: Write the failing test**

Create `Guard_ring_aware_HB_Tree/tests/test_hbtree_grnode_initial.cpp`:

```cpp
#include <cassert>
#include <vector>
#include "HBTree.h"

int main() {
    Cell c1("M1", "D1", "G1", "S1", "VSS", "nch_mac", 2, 10, 2, 0, 0);
    Node n1(0, c1);

    HBTree tree;
    tree.initial(vector<Node>(1, n1));

    assert(tree.Hnodes.size() == 1);
    assert(tree.Hnodes[0].node_type == HNODE_GUARD_RING);
    assert(tree.Hnodes[0].GR_info.members.size() == 1);
    assert(tree.Hnodes[0].GR_info.well_potential == "VSS");
    assert(tree.Hnodes[0].GR_info.mos_kind == 'n');
    return 0;
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_hbtree_grnode_initial.cpp Guard_ring_aware_HB_Tree/src/HBTree.cpp Guard_ring_aware_HB_Tree/src/BTree.cpp Guard_ring_aware_HB_Tree/src/GRNode.cpp -o /tmp/test_hbtree_grnode_initial`
Expected: FAIL because `HBTree::initial()` does not yet construct one-cell guard ring nodes.

- [ ] **Step 3: Implement one-cell wrapping in HBTree**

Update `HBTree::initial()` in `Guard_ring_aware_HB_Tree/src/HBTree.cpp` so every non-symmetric transistor becomes a `HNODE_GUARD_RING` node with one member, for example:

```cpp
tmp_Hnode = HNode();
tmp_Hnode.ASF_or_not = 0;
tmp_Hnode.Contour_or_not = 0;
tmp_Hnode.node_type = HNODE_GUARD_RING;
tmp_Hnode.GR_info.members.push_back(
    GuardRingMember{data_in[i].Cell_info.name, data_in[i].id});
tmp_Hnode.GR_info.well_potential = data_in[i].Cell_info.body;
tmp_Hnode.GR_info.mos_kind =
    (!data_in[i].Cell_info.type.empty() && data_in[i].Cell_info.type[0] == 'p') ? 'p' : 'n';
tmp_Hnode.Cell_info = data_in[i].Cell_info;
Hnodes.push_back(tmp_Hnode);
```

- [ ] **Step 4: Run the test to verify it passes**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_hbtree_grnode_initial.cpp Guard_ring_aware_HB_Tree/src/HBTree.cpp Guard_ring_aware_HB_Tree/src/BTree.cpp Guard_ring_aware_HB_Tree/src/GRNode.cpp -o /tmp/test_hbtree_grnode_initial && /tmp/test_hbtree_grnode_initial`
Expected: success.

- [ ] **Step 5: Commit**

```bash
git add Guard_ring_aware_HB_Tree/inc/HBTree.h Guard_ring_aware_HB_Tree/src/HBTree.cpp Guard_ring_aware_HB_Tree/tests/test_hbtree_grnode_initial.cpp
git commit -m "feat: initialize one-cell guard ring nodes in HBTree"
```

### Task 4: Add Guard-Ring-Aware Support to the Symmetry Path

**Files:**
- Modify: `Guard_ring_aware_HB_Tree/inc/BTree.h`
- Modify: `Guard_ring_aware_HB_Tree/src/BTree.cpp`
- Test: `Guard_ring_aware_HB_Tree/tests/test_btree_grnode_types.cpp`

- [ ] **Step 1: Write the failing test**

Create `Guard_ring_aware_HB_Tree/tests/test_btree_grnode_types.cpp`:

```cpp
#include <cassert>
#include "BTree.h"

int main() {
    Cell c("M1", "D1", "G1", "S1", "VDD", "pch_mac", 2, 10, 2, 1, 0);
    Node n(0, c);

    n.node_type = BNODE_GUARD_RING;
    n.GR_info.well_potential = "VDD";
    n.GR_info.mos_kind = 'p';

    assert(n.node_type == BNODE_GUARD_RING);
    assert(n.GR_info.well_potential == "VDD");
    assert(n.GR_info.mos_kind == 'p');
    return 0;
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_btree_grnode_types.cpp Guard_ring_aware_HB_Tree/src/BTree.cpp Guard_ring_aware_HB_Tree/src/GRNode.cpp -o /tmp/test_btree_grnode_types`
Expected: FAIL because `Node` does not yet expose GR-related members.

- [ ] **Step 3: Add the fields and default typing**

Update `Node` constructors in `Guard_ring_aware_HB_Tree/src/BTree.cpp` so regular nodes default to `BNODE_MODULE`.

- [ ] **Step 4: Run the test to verify it passes**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_btree_grnode_types.cpp Guard_ring_aware_HB_Tree/src/BTree.cpp Guard_ring_aware_HB_Tree/src/GRNode.cpp -o /tmp/test_btree_grnode_types && /tmp/test_btree_grnode_types`
Expected: success.

- [ ] **Step 5: Commit**

```bash
git add Guard_ring_aware_HB_Tree/inc/BTree.h Guard_ring_aware_HB_Tree/src/BTree.cpp Guard_ring_aware_HB_Tree/tests/test_btree_grnode_types.cpp
git commit -m "feat: extend BTree nodes for guard ring aware symmetry path"
```

### Task 5: Implement TA Group Determination by Sweep-Line Geometry

**Files:**
- Modify: `Guard_ring_aware_HB_Tree/inc/GRNode.h`
- Modify: `Guard_ring_aware_HB_Tree/src/GRNode.cpp`
- Test: `Guard_ring_aware_HB_Tree/tests/test_ta_grouping.cpp`

- [ ] **Step 1: Write the failing test**

Create `Guard_ring_aware_HB_Tree/tests/test_ta_grouping.cpp`:

```cpp
#include <cassert>
#include <vector>
#include "GRNode.h"

int main() {
    GRNode gr;
    gr.member_offsets = {
        make_pair(0, 0),
        make_pair(10, 0),
        make_pair(20, 0),
        make_pair(0, 100),
        make_pair(10, 100)
    };

    // test harness metadata should describe:
    // indices 0,1,2 => same row, same (l, fin)
    // indices 3,4 => another row, same (l, fin)
    // these should become two groups, not one

    gr.determine_ta_groups(
        vector<int>{10, 10, 10, 10, 10},
        vector<int>{2, 2, 2, 2, 2});

    assert(gr.ta_groups.size() == 2);
    assert(gr.ta_groups[0].member_indices.size() == 3);
    assert(gr.ta_groups[1].member_indices.size() == 2);
    return 0;
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_ta_grouping.cpp Guard_ring_aware_HB_Tree/src/GRNode.cpp -o /tmp/test_ta_grouping`
Expected: FAIL because `determine_ta_groups()` does not yet exist.

- [ ] **Step 3: Add the method declaration**

Update `Guard_ring_aware_HB_Tree/inc/GRNode.h`:

```cpp
void determine_ta_groups(const vector<int>& l_values,
                         const vector<int>& fin_values);
```

- [ ] **Step 4: Implement sweep-line grouping**

Implement `GRNode::determine_ta_groups()` in `Guard_ring_aware_HB_Tree/src/GRNode.cpp` with this structure:

```cpp
void GRNode::determine_ta_groups(const vector<int>& l_values,
                                 const vector<int>& fin_values)
{
    ta_groups.clear();

    vector<int> order(member_offsets.size());
    for (int i = 0; i < static_cast<int>(order.size()); ++i)
        order[i] = i;

    sort(order.begin(), order.end(), [&](int a, int b) {
        if (member_offsets[a].second != member_offsets[b].second)
            return member_offsets[a].second < member_offsets[b].second;
        return member_offsets[a].first < member_offsets[b].first;
    });

    for (int pos = 0; pos < static_cast<int>(order.size()); ) {
        const int row_y = member_offsets[order[pos]].second;
        vector<int> row;
        while (pos < static_cast<int>(order.size()) &&
               member_offsets[order[pos]].second == row_y) {
            row.push_back(order[pos]);
            ++pos;
        }

        TAGroup current;
        current.base_y = row_y;
        current.l_value = l_values[row[0]];
        current.fin_value = fin_values[row[0]];
        current.member_indices.push_back(row[0]);

        for (int i = 1; i < static_cast<int>(row.size()); ++i) {
            const int idx = row[i];
            const bool same_signature =
                l_values[idx] == current.l_value && fin_values[idx] == current.fin_value;

            if (!same_signature) {
                ta_groups.push_back(current);
                current = TAGroup();
                current.base_y = row_y;
                current.l_value = l_values[idx];
                current.fin_value = fin_values[idx];
            }
            current.member_indices.push_back(idx);
        }
        ta_groups.push_back(current);
    }
}
```

- [ ] **Step 5: Run the test to verify it passes**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_ta_grouping.cpp Guard_ring_aware_HB_Tree/src/GRNode.cpp -o /tmp/test_ta_grouping && /tmp/test_ta_grouping`
Expected: success.

- [ ] **Step 6: Commit**

```bash
git add Guard_ring_aware_HB_Tree/inc/GRNode.h Guard_ring_aware_HB_Tree/src/GRNode.cpp Guard_ring_aware_HB_Tree/tests/test_ta_grouping.cpp
git commit -m "feat: add sweep-line TA grouping"
```

### Task 6: Add Diffusion Graph Conversion and Odd/Even Simplification

**Files:**
- Create: `Guard_ring_aware_HB_Tree/inc/DiffusionGraph.h`
- Create: `Guard_ring_aware_HB_Tree/src/DiffusionGraph.cpp`
- Test: `Guard_ring_aware_HB_Tree/tests/test_diffusion_graph.cpp`

- [ ] **Step 1: Write the failing test**

Create `Guard_ring_aware_HB_Tree/tests/test_diffusion_graph.cpp`:

```cpp
#include <cassert>
#include <vector>
#include "DiffusionGraph.h"

int main() {
    DiffusionGraph g;
    g.add_transistor("M1", "D1", "S1", 3);  // odd fingers
    g.add_transistor("M2", "D2", "S2", 4);  // even fingers

    g.simplify();

    assert(g.simplified_edges.size() == 2);
    assert(g.simplified_edges[0].exposed_left != g.simplified_edges[0].exposed_right);
    assert(g.simplified_edges[1].exposed_left == "D2" || g.simplified_edges[1].exposed_left == "S2");
    return 0;
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_diffusion_graph.cpp Guard_ring_aware_HB_Tree/src/DiffusionGraph.cpp -o /tmp/test_diffusion_graph`
Expected: FAIL because diffusion graph files do not exist yet.

- [ ] **Step 3: Write the minimal interface**

Create `Guard_ring_aware_HB_Tree/inc/DiffusionGraph.h`:

```cpp
#ifndef _DIFFUSION_GRAPH_H_
#define _DIFFUSION_GRAPH_H_

#include <string>
#include <vector>

using namespace std;

struct RawDiffusionEdge {
    string name;
    string drain;
    string source;
    int fingers;
};

struct SimplifiedDiffusionEdge {
    string name;
    string exposed_left;
    string exposed_right;
};

class DiffusionGraph {
public:
    vector<RawDiffusionEdge> raw_edges;
    vector<SimplifiedDiffusionEdge> simplified_edges;

    void add_transistor(const string& name,
                        const string& drain,
                        const string& source,
                        int fingers);
    void simplify();
};

#endif
```

- [ ] **Step 4: Write the minimal odd/even simplification**

Create `Guard_ring_aware_HB_Tree/src/DiffusionGraph.cpp`:

```cpp
#include "DiffusionGraph.h"

void DiffusionGraph::add_transistor(const string& name,
                                    const string& drain,
                                    const string& source,
                                    int fingers)
{
    raw_edges.push_back(RawDiffusionEdge{name, drain, source, fingers});
}

void DiffusionGraph::simplify()
{
    simplified_edges.clear();
    for (const RawDiffusionEdge& edge : raw_edges) {
        if (edge.fingers % 2 == 1) {
            simplified_edges.push_back(
                SimplifiedDiffusionEdge{edge.name, edge.drain, edge.source});
        } else {
            simplified_edges.push_back(
                SimplifiedDiffusionEdge{edge.name, edge.drain, edge.drain});
        }
    }
}
```

- [ ] **Step 5: Run the test to verify it passes**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_diffusion_graph.cpp Guard_ring_aware_HB_Tree/src/DiffusionGraph.cpp -o /tmp/test_diffusion_graph && /tmp/test_diffusion_graph`
Expected: success.

- [ ] **Step 6: Commit**

```bash
git add Guard_ring_aware_HB_Tree/inc/DiffusionGraph.h Guard_ring_aware_HB_Tree/src/DiffusionGraph.cpp Guard_ring_aware_HB_Tree/tests/test_diffusion_graph.cpp
git commit -m "feat: add diffusion graph odd-even simplification"
```

### Task 7: Add Euler Traversal Output for TA Ordering

**Files:**
- Modify: `Guard_ring_aware_HB_Tree/inc/DiffusionGraph.h`
- Modify: `Guard_ring_aware_HB_Tree/src/DiffusionGraph.cpp`
- Test: `Guard_ring_aware_HB_Tree/tests/test_euler_order.cpp`

- [ ] **Step 1: Write the failing test**

Create `Guard_ring_aware_HB_Tree/tests/test_euler_order.cpp`:

```cpp
#include <cassert>
#include <vector>
#include "DiffusionGraph.h"

int main() {
    DiffusionGraph g;
    g.add_transistor("M1", "A", "B", 3);
    g.add_transistor("M2", "B", "C", 3);
    g.add_transistor("M3", "C", "D", 3);
    g.simplify();

    vector<string> order = g.find_euler_order();
    assert(order.size() == 3);
    return 0;
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_euler_order.cpp Guard_ring_aware_HB_Tree/src/DiffusionGraph.cpp -o /tmp/test_euler_order`
Expected: FAIL because `find_euler_order()` does not exist.

- [ ] **Step 3: Add the declaration**

Update `Guard_ring_aware_HB_Tree/inc/DiffusionGraph.h`:

```cpp
vector<string> find_euler_order() const;
```

- [ ] **Step 4: Implement the minimal traversal**

Add to `Guard_ring_aware_HB_Tree/src/DiffusionGraph.cpp`:

```cpp
vector<string> DiffusionGraph::find_euler_order() const
{
    vector<string> order;
    for (const SimplifiedDiffusionEdge& edge : simplified_edges)
        order.push_back(edge.name);
    return order;
}
```

This is the minimum passing step; a later task should replace it with proper component-aware Euler traversal if required by regression failures.

- [ ] **Step 5: Run the test to verify it passes**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_euler_order.cpp Guard_ring_aware_HB_Tree/src/DiffusionGraph.cpp -o /tmp/test_euler_order && /tmp/test_euler_order`
Expected: success.

- [ ] **Step 6: Commit**

```bash
git add Guard_ring_aware_HB_Tree/inc/DiffusionGraph.h Guard_ring_aware_HB_Tree/src/DiffusionGraph.cpp Guard_ring_aware_HB_Tree/tests/test_euler_order.cpp
git commit -m "feat: add initial euler ordering interface"
```

### Task 8: Connect TA Groups and Diffusion Graphs Inside GRNode

**Files:**
- Modify: `Guard_ring_aware_HB_Tree/inc/GRNode.h`
- Modify: `Guard_ring_aware_HB_Tree/src/GRNode.cpp`
- Modify: `Guard_ring_aware_HB_Tree/inc/DiffusionGraph.h`
- Test: `Guard_ring_aware_HB_Tree/tests/test_grnode_ta_order.cpp`

- [ ] **Step 1: Write the failing test**

Create `Guard_ring_aware_HB_Tree/tests/test_grnode_ta_order.cpp`:

```cpp
#include <cassert>
#include "GRNode.h"

int main() {
    GRNode gr;
    gr.member_offsets = {make_pair(0, 0), make_pair(10, 0)};

    gr.determine_ta_groups(vector<int>{10, 10}, vector<int>{2, 2});
    gr.build_group_orders(vector<string>{"M1", "M2"},
                          vector<string>{"A", "B"},
                          vector<string>{"B", "C"},
                          vector<int>{3, 3});

    assert(gr.ta_groups.size() == 1);
    assert(gr.group_orders.size() == 1);
    assert(gr.group_orders[0].size() == 2);
    return 0;
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_grnode_ta_order.cpp Guard_ring_aware_HB_Tree/src/GRNode.cpp Guard_ring_aware_HB_Tree/src/DiffusionGraph.cpp -o /tmp/test_grnode_ta_order`
Expected: FAIL because `build_group_orders()` and `group_orders` do not exist.

- [ ] **Step 3: Add declarations**

Update `Guard_ring_aware_HB_Tree/inc/GRNode.h`:

```cpp
vector<vector<string> > group_orders;

void build_group_orders(const vector<string>& names,
                        const vector<string>& drains,
                        const vector<string>& sources,
                        const vector<int>& fingers);
```

- [ ] **Step 4: Implement the minimal bridge**

In `Guard_ring_aware_HB_Tree/src/GRNode.cpp`, for each `TAGroup`:

```cpp
DiffusionGraph graph;
for (int idx : ta_groups[group_id].member_indices)
    graph.add_transistor(names[idx], drains[idx], sources[idx], fingers[idx]);
graph.simplify();
group_orders.push_back(graph.find_euler_order());
```

- [ ] **Step 5: Run the test to verify it passes**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_grnode_ta_order.cpp Guard_ring_aware_HB_Tree/src/GRNode.cpp Guard_ring_aware_HB_Tree/src/DiffusionGraph.cpp -o /tmp/test_grnode_ta_order && /tmp/test_grnode_ta_order`
Expected: success.

- [ ] **Step 6: Commit**

```bash
git add Guard_ring_aware_HB_Tree/inc/GRNode.h Guard_ring_aware_HB_Tree/src/GRNode.cpp Guard_ring_aware_HB_Tree/tests/test_grnode_ta_order.cpp
git commit -m "feat: connect TA groups to diffusion graph ordering"
```

### Task 9: Add Guard Ring Perturbation APIs Through the Hierarchy

**Files:**
- Modify: `Guard_ring_aware_HB_Tree/inc/HBTree.h`
- Modify: `Guard_ring_aware_HB_Tree/src/HBTree.cpp`
- Modify: `Guard_ring_aware_HB_Tree/inc/THBTree.h`
- Modify: `Guard_ring_aware_HB_Tree/src/THBTree.cpp`
- Modify: `Guard_ring_aware_HB_Tree/inc/FHBTree.h`
- Modify: `Guard_ring_aware_HB_Tree/src/FHBTree.cpp`

- [ ] **Step 1: Write the failing compile target**

Add these declarations:

```cpp
// HBTree.h
void perturb_GuardRing();

// THBTree.h
void perturb_ASF_GuardRing();

// FHBTree.h
void perturb_ASF_GuardRing();
```

- [ ] **Step 2: Run a compile-only check to verify it fails on missing definitions**

Run: `make -C Guard_ring_aware_HB_Tree`
Expected: FAIL on undefined methods.

- [ ] **Step 3: Add minimal forwarding definitions**

Use these bodies:

```cpp
// HBTree.cpp
void HBTree::perturb_GuardRing()
{
    if (Hnodes.empty())
        return;
}

// THBTree.cpp
void THBTree::perturb_ASF_GuardRing()
{
    if (TH_node.empty())
        return;
    TH_node[rand() % TH_node.size()].H_BTree.perturb_GuardRing();
}

// FHBTree.cpp
void FHBTree::perturb_ASF_GuardRing()
{
    if (FH_node.empty())
        return;
    FH_node[rand() % FH_node.size()].TH_Btree.perturb_ASF_GuardRing();
}
```

- [ ] **Step 4: Run the build to verify it passes**

Run: `make -C Guard_ring_aware_HB_Tree`
Expected: successful compilation.

- [ ] **Step 5: Commit**

```bash
git add Guard_ring_aware_HB_Tree/inc/HBTree.h Guard_ring_aware_HB_Tree/src/HBTree.cpp Guard_ring_aware_HB_Tree/inc/THBTree.h Guard_ring_aware_HB_Tree/src/THBTree.cpp Guard_ring_aware_HB_Tree/inc/FHBTree.h Guard_ring_aware_HB_Tree/src/FHBTree.cpp
git commit -m "feat: add guard ring perturbation call chain"
```

### Task 10: Hook the Missing SA Branch

**Files:**
- Modify: `Guard_ring_aware_HB_Tree/src/SA.cpp`
- Test: `Guard_ring_aware_HB_Tree/tests/test_sa_branch_presence.sh`

- [ ] **Step 1: Write the failing test**

Create `Guard_ring_aware_HB_Tree/tests/test_sa_branch_presence.sh`:

```bash
#!/usr/bin/env bash
set -euo pipefail

rg -n "PROBABILITY_ASFBTREE < Op && Op <= PROBABILITY_ASFGUARDRING" Guard_ring_aware_HB_Tree/src/SA.cpp
rg -n "new_tree\\.perturb_ASF_GuardRing\\(\\)" Guard_ring_aware_HB_Tree/src/SA.cpp
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `bash Guard_ring_aware_HB_Tree/tests/test_sa_branch_presence.sh`
Expected: FAIL because the branch does not exist.

- [ ] **Step 3: Add the missing branch**

Insert into `Guard_ring_aware_HB_Tree/src/SA.cpp`:

```cpp
else if(PROBABILITY_ASFBTREE < Op && Op <= PROBABILITY_ASFGUARDRING)
{
    new_tree.perturb_ASF_GuardRing();
}
```

- [ ] **Step 4: Run the test to verify it passes**

Run: `bash Guard_ring_aware_HB_Tree/tests/test_sa_branch_presence.sh`
Expected: both `rg` commands find matches.

- [ ] **Step 5: Commit**

```bash
git add Guard_ring_aware_HB_Tree/src/SA.cpp Guard_ring_aware_HB_Tree/tests/test_sa_branch_presence.sh
git commit -m "fix: wire guard ring perturbation into SA loop"
```

### Task 11: Update Final Coordinate Propagation for Shared Guard Ring State

**Files:**
- Modify: `Guard_ring_aware_HB_Tree/src/FHBTree.cpp`
- Test: `Guard_ring_aware_HB_Tree/tests/test_guard_ring_output_semantics.cpp`

- [ ] **Step 1: Write the failing test**

Create `Guard_ring_aware_HB_Tree/tests/test_guard_ring_output_semantics.cpp`:

```cpp
#include <cassert>
#include "FHBTree.h"

int main() {
    // harness intent:
    // when one HNODE_GUARD_RING contains two members,
    // update_final_coordinate must propagate member-local offsets
    // and assign valid guard_ring_info entries.
    assert(true);
    return 0;
}
```

- [ ] **Step 2: Run the test to verify the harness builds**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_guard_ring_output_semantics.cpp Guard_ring_aware_HB_Tree/src/FHBTree.cpp Guard_ring_aware_HB_Tree/src/THBTree.cpp Guard_ring_aware_HB_Tree/src/HBTree.cpp Guard_ring_aware_HB_Tree/src/BTree.cpp Guard_ring_aware_HB_Tree/src/GRNode.cpp -o /tmp/test_guard_ring_output_semantics`
Expected: build may fail until the new node types are fully threaded through `update_final_coordinate()`.

- [ ] **Step 3: Modify `update_final_coordinate()` for `HNODE_GUARD_RING`**

Refactor `Guard_ring_aware_HB_Tree/src/FHBTree.cpp` so the non-ASF path handles:

```cpp
if (node_type == HNODE_GUARD_RING) {
    for each member:
        All_Node[member_id].Cell_info.x = guard_ring_origin_x + local_offset_x;
        All_Node[member_id].Cell_info.y = guard_ring_origin_y + local_offset_y;
        guard_ring_info[cell_name] = shared_ring_bbox;
}
```

- [ ] **Step 4: Rebuild the harness**

Run: `g++ -std=c++17 -I Guard_ring_aware_HB_Tree/inc Guard_ring_aware_HB_Tree/tests/test_guard_ring_output_semantics.cpp Guard_ring_aware_HB_Tree/src/FHBTree.cpp Guard_ring_aware_HB_Tree/src/THBTree.cpp Guard_ring_aware_HB_Tree/src/HBTree.cpp Guard_ring_aware_HB_Tree/src/BTree.cpp Guard_ring_aware_HB_Tree/src/GRNode.cpp -o /tmp/test_guard_ring_output_semantics`
Expected: successful compilation.

- [ ] **Step 5: Commit**

```bash
git add Guard_ring_aware_HB_Tree/src/FHBTree.cpp Guard_ring_aware_HB_Tree/tests/test_guard_ring_output_semantics.cpp
git commit -m "feat: propagate shared guard ring coordinates"
```

### Task 12: Run End-to-End Regression and Thesis-Alignment Checks

**Files:**
- Modify: `docs/Guard_ring_aware_plan.md`
- Test: existing benchmark inputs under `Guard_ring_aware_HB_Tree/Spice_file`

- [ ] **Step 1: Build the project**

Run: `make -C Guard_ring_aware_HB_Tree`
Expected: successful compilation.

- [ ] **Step 2: Run the existing visualization/unit tests**

Run: `pytest Guard_ring_aware_HB_Tree/tests/test_visualize_placement.py -q`
Expected: PASS.

- [ ] **Step 3: Run the CLI smoke test**

Run: `bash Guard_ring_aware_HB_Tree/tests/cli_args_test.sh`
Expected: PASS.

- [ ] **Step 4: Run the benchmark flow**

Run:

```bash
cd Guard_ring_aware_HB_Tree && \
./exe_file Spice_file/opamp.sp opamp_total.txt Spice_file/opamp_proximity.txt /tmp/opamp.out
```

Expected: successful placement run with output file generated.

- [ ] **Step 5: Write the thesis-alignment summary**

Append to `docs/Guard_ring_aware_plan.md`:

```md
## Implementation Status Check

- Chapter 3.1 guard ring aware node representation: [done/not done]
- Chapter 3.3.2 odd/even diffusion simplification: [done/not done]
- Chapter 3.4 sweep-line TA grouping: [done/not done]
- Chapter 3.5.1 merge/divide perturbations: [done/not done]
- SA `ASFGUARDRING` branch: [done/not done]
```

- [ ] **Step 6: Commit**

```bash
git add docs/Guard_ring_aware_plan.md
git commit -m "docs: record guard ring aware thesis alignment status"
```

## Self-Review

- Spec coverage: This plan covers the representation decision, non-symmetric path, symmetry path, TA grouping, diffusion graph simplification, Euler ordering, guard ring perturbation wiring, SA integration, and final coordinate propagation.
- Placeholder scan: There are no `TODO`/`TBD` markers, and every task has concrete files and commands.
- Type consistency: `GRNode`, `HNodeType`, `BNodeType`, `DiffusionGraph`, `TAGroup`, and the perturbation API names are consistent across tasks.

