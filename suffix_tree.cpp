#include <map>
#include <set>
#include <list>
#include <stack>
#include <iostream>
#include <algorithm>

using namespace std;

// Used to define direction of an Edge and suffix link.
struct Node
{
    Node() : parent(0), depth(0), suffixLink(-1){}

    Node* parent;
    int depth;
    int suffixLink;
    char firstEdgeChar;

    // Represents different leafs of branch node.
    set<int> leafKinds;

    // Used to identify Nodes in an array.
    static size_t count;
};

size_t Node::count = 1; // Root is already there.

// Active/end points are referenced like this: (node, (l, r)).
struct ReferencePair
{
    ReferencePair(int node, int left, int right)
        : node(node), left(left), right(right)   { }

    bool implicit()
    {
        return left <= right;
    }

    void canonize();

    int node;
    int left, right; // Indices of the active/end point.
};

// Input string.
string s;
// Enough for the task.
const size_t MAXNODES = 250000;
// When STree will be created MAXNODES ctors will be called.
Node nodes[MAXNODES];

// Edge represents the connection between nodes in a tree.
// Edge always has 2 nodes on it's both sides.
// Edge always contains non-empty string.
// The parent-child hierarchy of connected nodes
// defines the direction of an edge.
struct Edge
{
    Edge() : parentNode(-1) {}  // Not-in-a-tree edge.
    // Edges are created during traversal from active point to end point.
    Edge(int parent, int left, int right) : left(left),
                                            right(right),
                                            parentNode(parent),
                                            childNode(Node::count++)
    {
        nodes[childNode].parent = &nodes[parentNode];
        nodes[childNode].firstEdgeChar = s[left];
    }
    Edge(int parent, int child,
         int left, int right) : left(left), right(right),
                                parentNode(parent), childNode(child)
    {
        nodes[childNode].parent = &nodes[parentNode];
        nodes[childNode].firstEdgeChar = s[left];
    }

    // Splits the edge by creating new edge and an init if suffixLink.
    int splitEdge(const ReferencePair&);

    int left, right;           // The string on the edge.
    int parentNode, childNode;
};

// Any edge can be retrieved by it's parent node
// and the first character: edges[pair(parent, char)].
map<pair<int, char>, Edge> edges;

// Current edge already identifies itself in edges map
// with the pair (parent, firstChar). To leave it valid,
// the new edge will be descendant of this one.
int Edge::splitEdge(const ReferencePair& activePoint)
{
    int newNode = Node::count++;

    // Descendant.
    size_t leftOfNewEdge = left + activePoint.right - activePoint.left + 1;
    Edge newEdge(newNode, childNode, leftOfNewEdge, right);
    edges[make_pair(newNode, s[leftOfNewEdge])] = newEdge;

    // This edge is shortened.
    right = left + activePoint.right - activePoint.left;
    childNode = newNode;

    // newNode is parented from this edge parent.
    nodes[newNode].parent = &nodes[parentNode];
    nodes[newNode].firstEdgeChar = s[left];
    nodes[newNode].depth = nodes[parentNode].depth + right - left + 1;

    return childNode; // It's not a leaf now.
}

// Finds the closest ancestor of the node presented
// by this reference pair. May change only 'node' and 'left'.
void ReferencePair::canonize()
{
    if (implicit())
    {
        Edge* edge = &edges[make_pair(node, s[left])];
        int edgeSpan = edge->right - edge->left;   // 0 or bigger.
        while (edgeSpan <= (right - left))
        {
            left += (edgeSpan + 1);   // So begins the next edge.
            node = edge->childNode;
            // If the path defines implicit node.
            if (left <= right)
            {
                edge = &edges[make_pair(node, s[left])];
                edgeSpan = edge->right - edge->left;
            }
        }
    }
}

size_t K;
size_t wordsSize[10]; // Length of each word.
const char* terminators[] = {"$", "#", "@", "^", "&", "|", "*", "%", "(", ")"};
// Leafs for each word.
map<size_t, list<Node*> > leafGroups;

struct STree
{
    STree() : currentWord(0) {}

    void buildTree();
    void update(ReferencePair&, size_t);
    const Edge& findEdge(int, char);

    int currentWord; // Word that is inserted in the tree for the moment.
};

void STree::buildTree()
{
    ReferencePair activePoint(0, 0, -1);
    for (size_t i = 0; i < s.size(); i++)
        update(activePoint, i);
}

// Add new character in a tree by updating boundary path
// from an active point to an end point. Returned end point
// will we the active point for the next update.
void STree::update(ReferencePair& activePoint, size_t i)
{
    int parentNode, lastParentNode = 0;
    while (true)
    {
        // Is this an end point? (test-and-split)
        if (activePoint.implicit())
        {
            Edge* edge = &edges[make_pair(activePoint.node, s[activePoint.left])];
            size_t span = activePoint.right - activePoint.left; // >= 0.
            if (s[edge->left + span + 1] == s[i])
                break;
            // This parent is pointing to it's own parent now.
            parentNode = edge->splitEdge(activePoint);
        }
        else
        {
            // If an edge begins with s[i] than it's the end point.
            if (edges.count(make_pair(activePoint.node, s[i])) > 0)
                break;
            parentNode = activePoint.node;
        }

        // It's not. Create leaf edge.
        Edge edge(parentNode, i, s.size() - 1);
        edges[make_pair(parentNode, s[i])] = edge;

        // Collect new leaf in it's group.
        leafGroups[currentWord].push_back(&nodes[edge.childNode]);

        // If the last time we created an internal node
        // make it point with the suffix link to this node.
        if (lastParentNode > 0)
            nodes[lastParentNode].suffixLink = parentNode;
        lastParentNode = parentNode;

        // Go to the next suffix from bounder path.
        if (activePoint.node == 0)
            activePoint.left++; // Root doesn't have a suffix link.
        else
            activePoint.node = nodes[activePoint.node].suffixLink;
        activePoint.canonize();

        // Begining of the next word.
        if (activePoint.node == 0 && !activePoint.implicit() &&
                                     s[i] == terminators[currentWord][0])
            currentWord++; // Terminate symbol will be as the next word leaf.
    }

    // oldr != root
    if (lastParentNode != 0)
        nodes[lastParentNode].suffixLink = activePoint.node;

    // Make the end point the next active point.
    activePoint.right++;
    activePoint.canonize();
}

bool comp (const Node& a, const Node& b) { return a.depth > b.depth; }

int main()
{
    cin >> K;

    if (K == 1)
    {
        string str;
        cin >> str;
        cout << str;
        return 0;
    }

    string generalized;
    for (size_t i = 0; i < K; i++)
    {
        string str;
        cin >> str;

        wordsSize[i] = str.size() + 1;
        s += str + terminators[i];
    }

    STree tree;
    tree.buildTree();

    map<size_t, list<Node*> >::iterator it;
    for (it = leafGroups.begin(); it != leafGroups.end(); ++it)
    {
        list<Node*>::iterator leafNode;
        for (leafNode = it->second.begin(); leafNode != it->second.end(); ++leafNode)
        {
            Node* node = *leafNode;
            while (node->parent != 0)
            {
                node = node->parent;
                node->leafKinds.insert(it->first);
            }
        }
    }

    Node* deepestNode = 0;
    for (size_t i = 0; i < Node::count; i++)
    {
        if (nodes[i].leafKinds.size() < K)
            continue;

        if (deepestNode != 0 && nodes[i].depth > deepestNode->depth)
            deepestNode = &nodes[i];
        else if (deepestNode == 0)
            deepestNode = &nodes[i];
    }

    if (deepestNode == 0)
        cout << "";
    else
    {
        stack<char> path;
        while (deepestNode->parent)
        {
            path.push(deepestNode->firstEdgeChar);
            deepestNode = deepestNode->parent;
        }

        int node = 0; // Root.
        while (!path.empty())
        {
            Edge* edge = &edges[make_pair(node, path.top())];

            for (int i = edge->left; i <= edge->right; i++)
                cout << s[i];

            path.pop();
            node = edge->childNode;
        }
    }
}
