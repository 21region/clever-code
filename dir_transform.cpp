#include <map>
#include <stack>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

struct Node
{
    Node(const string&, size_t);
    ~Node();

    Node* addChild(Node*);
    Node* addChild(const string&, size_t);
    Node* pythonNext();
    static Node* findNext();

    Node* parent;
    Node* lChild;
    Node* rChild;
    Node* lSibling;
    Node* rSibling;

    size_t id;
    string name;
    size_t depth;
    size_t amountOfChildren;

    // Static
    static void deleteTree();

    static map<size_t, Node*> nodesById;
};

map<size_t, Node*> Node::nodesById;

Node::Node(const string& name, size_t id) : parent(0), lChild(0), rChild(0),
                                            lSibling(0), rSibling(0), id(id),
                                            name(name), depth(0),
                                            amountOfChildren(0)
{
    nodesById[id] = this;
}

Node::~Node()
{
    if (lChild)
    {
        while (lChild)
        {
            Node* sibling = lChild->rSibling;
            delete lChild;
            lChild = sibling;
        }
    }
}

void Node::deleteTree()
{
    if (nodesById.size() > 0)
        delete nodesById[0];
}

Node* Node::addChild(const string& name, size_t id)
{
    Node* child = new Node(name, id);

    child->parent = this;
    child->depth = depth + 1;

    if (lChild)
    {
        child->rSibling = lChild;
        lChild->lSibling = child;
    }
    else
        rChild = child;

    lChild = child;

    amountOfChildren++;

    return child;
}

Node* Node::addChild(Node* child)
{
    child->parent = this;
    child->depth = depth + 1;

    if (lChild)
    {
        child->rSibling = lChild;
        lChild->lSibling = child;
    }
    else
        rChild = child;

    lChild = child;

    amountOfChildren++;

    return child;
}

Node* Node::pythonNext()
{
    if (rChild)
        return rChild;

    if (lSibling)
        return lSibling;

    Node* tempParent = parent;
    Node* sibling = tempParent->lSibling;
    while (!sibling && tempParent->parent)
    {
        tempParent = tempParent->parent;
        sibling = tempParent->lSibling;
    }

    return sibling;
}

Node* Node::findNext()
{
    static map<size_t, Node*>::iterator it = nodesById.begin();

    if (++it != nodesById.end())
        return it->second;
    else
        return 0;
}
//// End of Node structure definition. /////////////////////////////////////////


//// Functions for constructing tree. //////////////////////////////////////////
void buildFindTree()
{
    size_t n;
    cin >> n;

    size_t id;
    string path;
    map<string, Node*> nodeMap;
    for (size_t i = 0; i < n; i++)
    {
        cin >> path >> id;

        size_t rightMostSlash = path.rfind('/');
        size_t leftSlash = path.rfind('/', rightMostSlash - 1);

        if (rightMostSlash == string::npos)
        {
            Node* root = new Node(path, id);
            nodeMap[path] = root;
        }
        else
        {
            Node* parent;
            if (leftSlash == string::npos)
                parent = nodeMap[path.substr(0, rightMostSlash)];
            else
                parent = nodeMap[path.substr(leftSlash + 1,
                                             rightMostSlash - leftSlash - 1)];

            string childName = path.substr(rightMostSlash + 1);
            Node* child = parent->addChild(childName, id);
            nodeMap[childName] = child;
        }
    }
}

void buildPythonTree()
{
    size_t n;
    cin >> n;
    cin.ignore();       // Discard '\n'.

    // Depth of the previous node.
    size_t prevDepth = 0;
    // Last node at depth j. Used for finding parent.
    vector<Node*> lastNode(n);

    // Build tree and fill nodesById.
    for (size_t i = 0; i < n; i++)
    {
        string s;       // File|Folder name + id.
        getline(cin, s);

        size_t nameBegPos = s.find_first_not_of(' ');
        size_t nameEndPos = s.rfind(' ');

        size_t id;
        stringstream ss;
        ss << s.substr(nameEndPos + 1);
        ss >> id;

        if (id == 0) // Root
        {
            Node* root = new Node(s.substr(0, s.size() - 2), id);
            lastNode[0] = root;
            prevDepth = 0;
        }
        else
        {
            size_t depth = nameBegPos / 4;
            Node* child = 0;
            Node* parent = 0;

            if (depth > prevDepth) // Parent is last node at prevDepth.
                parent = lastNode[prevDepth];
            else // Last node at a lesser depth. depth <= prevDepth.
                parent = lastNode[depth - 1];

            child = parent->addChild(s.substr(nameBegPos,
                                     nameEndPos - nameBegPos), id);
            prevDepth = depth;
            lastNode[depth] = child;
        }
    }
}

void buildAcm1Tree()
{
    size_t n;
    cin >> n;

    for (size_t i = 0; i < n; i++)
    {
        size_t id;
        string name;
        cin >> name >> id;

        // Store nodes in a static map. They do not form a tree now.
        new Node(name, id);
    }

    map<size_t, Node*>::iterator it;
    for (it = Node::nodesById.begin(); it != Node::nodesById.end(); ++it)
    {
        size_t amountOfChildren;
        cin >> amountOfChildren;

        while (amountOfChildren)
        {
            size_t id;
            cin >> id;
            it->second->addChild(Node::nodesById[id]);
            amountOfChildren--;
        }
    }
}

void buildAcm2Tree()
{
    size_t n;
    cin >> n;

    for (size_t i = 0; i < n; i++)
    {
        size_t id;
        string name;
        cin >> name >> id;

        new Node(name, id);
    }

    map<size_t, Node*>::iterator it;
    for (it = Node::nodesById.begin(); it != Node::nodesById.end(); ++it)
    {
        int parentID;
        cin >> parentID;

        if (parentID != -1)
            Node::nodesById[parentID]->addChild(it->second);
    }
}

void buildAcm3Tree()
{
    size_t n;
    cin >> n;

    for (size_t i = 0; i < n; i++)
    {
        size_t id;
        string name;
        cin >> name >> id;

        new Node(name, id);
    }

    for (size_t i = 0; i < n - 1; i++)
    {
        size_t parentID, childID;
        cin >> parentID >> childID;

        Node::nodesById[parentID]->addChild(Node::nodesById[childID]);
    }
}

void buildXmlTree()
{
    cin.ignore(); // Get rid of '\n'.

    size_t openDirTags = 0;
    vector<Node*> lastDirNode;
    do
    {
        string tag;
        getline(cin, tag);

        if (tag[2 * openDirTags + 1] == 'd') // Open <dir> tag.
        {
            size_t begNamePos = 2 * openDirTags + 11;
            size_t endNamePos = tag.find('\'', begNamePos);

            string name = tag.substr(begNamePos, endNamePos - begNamePos);

            size_t idBegPos = endNamePos + 6;
            size_t idEndPos = tag.size() - 2;

            size_t id;
            stringstream ss;
            ss << tag.substr(idBegPos, idEndPos - idBegPos);
            ss >> id;

            Node* node = new Node(name, id); // Directory node.

            if (openDirTags > 0)
                lastDirNode[openDirTags - 1]->addChild(node);

            if (lastDirNode.size() > openDirTags)
                lastDirNode[openDirTags] = node;
            else
                lastDirNode.push_back(node);

            openDirTags++;
        }
        else if (tag[2 * openDirTags + 1] == 'f') // <file> tag.
        {
            size_t begNamePos = 2 * openDirTags + 12;
            size_t endNamePos = tag.find('\'', begNamePos);

            string name = tag.substr(begNamePos, endNamePos - begNamePos);

            size_t idBegPos = endNamePos + 6;
            size_t idEndPos = tag.size() - 3;

            size_t id;
            stringstream ss;
            ss << tag.substr(idBegPos, idEndPos - idBegPos);
            ss >> id;

            Node* node = new Node(name, id); // File node.

            lastDirNode[openDirTags - 1]->addChild(node);
        }
        else                    // </dir> tag.
            openDirTags--;

    } while (openDirTags > 0);
}

//// Functions for outputting tree. ////////////////////////////////////////////

void outputFindTree()
{
    cout << Node::nodesById.size() << '\n';

    stack<string> names;
    Node* node = Node::nodesById[0];
    cout << node->name << ' ' << node->id << '\n';
    while ((node = Node::findNext()))
    {
        size_t id = node->id;
        while (node)
        {
            names.push(node->name);
            node = node->parent;
        }
        while (names.size() > 1)
        {
            cout << names.top() << '/';
            names.pop();
        }
        cout << names.top() << ' ' << id << "\n";
        names.pop();
    }
}

void outputPythonTree()
{
    const string space = "    ";

    cout << Node::nodesById.size() << "\n"
         << Node::nodesById[0]->name << ' ' << 0 << "\n";

    Node* node = Node::nodesById[0]; // Root.
    while ((node = node->pythonNext()))
    {
        for (size_t i = 1; i <= node->depth; i++)
            cout << space;

        cout << node->name << ' ' << node->id << "\n";
    }
}

void outputAcm1Tree()
{
    cout << Node::nodesById.size() << '\n';

    map<size_t, Node*>::iterator it;
    for (it = Node::nodesById.begin(); it != Node::nodesById.end(); ++it)
        cout << it->second->name << ' ' << it->first << '\n';

    for (it = Node::nodesById.begin(); it != Node::nodesById.end(); ++it)
    {
        cout << it->second->amountOfChildren << ' ';

        Node* child = it->second->rChild;
        while (child)
        {
            cout << child->id << ' ';
            child = child->lSibling;
        }
        cout << '\n';
    }
}

void outputAcm2Tree()
{
    cout << Node::nodesById.size() << '\n';

    map<size_t, Node*>::iterator it;
    for (it = Node::nodesById.begin(); it != Node::nodesById.end(); ++it)
        cout << it->second->name << ' ' << it->first << '\n';

    for (it = Node::nodesById.begin(); it != Node::nodesById.end(); ++it)
    {
        Node* parent = it->second->parent;
        if (parent)
            cout << parent->id << '\n';
        else
            cout << -1 << '\n';
    }
}

void outputAcm3Tree()
{
    cout << Node::nodesById.size() << '\n';

    map<size_t, Node*>::iterator it;
    for (it = Node::nodesById.begin(); it != Node::nodesById.end(); ++it)
        cout << it->second->name << ' ' << it->first << '\n';

    for (it = Node::nodesById.begin(); it != Node::nodesById.end(); ++it)
    {
        Node* parent = it->second;
        Node* child = parent->rChild;
        while (child)
        {
            cout << parent->id << ' ' << child->id << '\n';
            child = child->lSibling;
        }
    }
}

void outputXmlTree()
{
    const string space = "  ";

    size_t lastDepth = 0;
    Node* node = Node::nodesById[0];
    do
    {
        while (lastDepth > node->depth)
        {
            for (size_t i = 0; i < lastDepth - 1; i++)
                cout << space;
            cout << "</dir>\n";
            lastDepth--;
        }

        for (size_t i = 0; i < node->depth; i++)
            cout << space;

        if (node->rChild)
            cout << "<dir name=\'" << node->name
                 << "\' id=\'" << node->id << "\'>\n";
        else
            cout << "<file name=\'" << node->name
                 << "\' id=\'" << node->id << "\'/>\n";

        lastDepth = node->depth;
    } while ((node = node->pythonNext()));

    while (lastDepth > 0)
    {
        for (size_t i = 0; i < lastDepth - 1; i++)
            cout << space;
        cout << "</dir>\n";
        lastDepth--;
    }
}

int main()
{
    string inFormat, outFormat;
    cin >> inFormat >> outFormat;

    // Build tree from input.
    if (inFormat == "find")
        buildFindTree();
    else if (inFormat == "python")
        buildPythonTree();
    else if (inFormat == "acm1")
        buildAcm1Tree();
    else if (inFormat == "acm2")
        buildAcm2Tree();
    else if (inFormat == "acm3")
        buildAcm3Tree();
    else if (inFormat == "xml")
        buildXmlTree();

    // Output built tree.
    if (outFormat == "find")
        outputFindTree();
    else if (outFormat == "python")
        outputPythonTree();
    else if (outFormat == "acm1")
        outputAcm1Tree();
    else if (outFormat == "acm2")
        outputAcm2Tree();
    else if (outFormat == "acm3")
        outputAcm3Tree();
    else if (outFormat == "xml")
        outputXmlTree();

    Node::deleteTree();
}
