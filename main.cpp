#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>

using namespace std;

unsigned int hashFunction(char const* key, int table_size)
{
    int b = 0, count = 0;
    unsigned int hashCode = 0;
    for (char c = *key; c; c =* ++key){
        if (count % 2 == 0){
            b=(hashCode << 7) ^ c ^ (hashCode >> 3);
        }
        else{
            b = (hashCode << 11) ^ c ^ (hashCode >> 5);
            b = ~b;
        }
        hashCode = hashCode ^ b;
        count++;
    }
    hashCode = hashCode | (1 << 32);
    return hashCode % table_size;
}

/** node class **/
struct Node {
    string key;
    string name;
    Node *next;

    ///constructor for node
    Node()
    {
        this->name = "";
        this->key = "";
        next = nullptr;
    }

    Node(string key, string name) {
        this->name = name;
        this->key = key;
        next = nullptr;
    };

    ///operator override
    Node &operator=(const Node &node) {
        this->key = node.key;
        this->name = node.name;
        this->next = node.next;
        return *this;
    }
};

void addNode(string key, string name, Node*& node) {     ///node means head node inside vector.
    if (node->key == "") ///if head is not exist, set head first.
    {
        node->key = key;
        node->name = name;
    }else
    {
        Node* newNode = new Node(key, name);
        newNode->next = node;
        node = newNode;
    }
}
 Node* findNode(string key, Node* node) ///node means head node inside vector.
 {
    while(node && node->key != "")
    {
        if (node->key == key)
            return node;
        node = node->next;
    }
     return nullptr;
 }

 void deleteNode(string key, Node*& firstNode) ///firstNode means head node inside vector.
 {
    Node* node = findNode(key, firstNode); ///find the node needs to be deleted.
    if (node) ///first confirm that the node is exists
    {
        node->key = firstNode->key;
        node->name = firstNode->name;
        Node* temp = firstNode;
        firstNode = firstNode->next;
        delete temp;
    }
 }


/** map class **/
class UnorderedMap
{
private:
    //define your data structure here
    vector<Node*> bucket; /// bucket structure
    //define other attributes e.g. bucket count, maximum load factor, size of table, etc.
    unsigned int bucketCount;
    double LF;
    double currLF;
    int currentSize;

public:
    class Iterator;
    UnorderedMap(unsigned int bucketCount, double loadFactor);
    ~UnorderedMap();
    Iterator begin() const;
    Iterator end() const;
    std::string& operator[] (std::string const& key);
    void rehash();
    void remove(std::string const& key);
    unsigned int size();
    double loadFactor();

    /** iterator calss **/
    class Iterator
    {
        Node* node;
        int currentBucket;
        const UnorderedMap* map;
    public:
        //this constructor does not need to be a default constructor;
        //the parameters for this constructor are up to your discretion.
        //hint: you may need to pass in an UnorderedMap object.
        Iterator(int currentBucket, const UnorderedMap* map) ///constructor
        {
            this->node = map->bucket[currentBucket];
            this->map = map;
            this->currentBucket = currentBucket;
        }
        Iterator(Node* node) ///constructor
        {
            this->node = node;
            this->map = nullptr;
            this->currentBucket = 0;
        }
        Iterator& operator=(Iterator const& rhs){
            this->node->key = rhs.node->key;
            this->node->name = rhs.node->name;
            this->node->next = rhs.node->next;
            this->currentBucket = rhs.currentBucket;
        }
        Iterator& operator++()
        {
            if (node->key != "")
                node = node->next;

            while ((!node || node->key == "") && ++currentBucket < map->bucket.size())
                node = map->bucket.at(currentBucket);
            return *this;
        }
        bool operator!=(Iterator const& rhs) {return rhs.node != this->node;}
        bool operator==(Iterator const& rhs) {return rhs.node == this->node;}
        std::pair<std::string, std::string> operator*() const {return make_pair(node->key, node->name);}
        friend class UnorderedMap;
    };
};

UnorderedMap::UnorderedMap(unsigned int bucketCount, double loadFactor) ///constructor for unordered map
{
    this->bucketCount = bucketCount;
    LF = loadFactor;
    currLF = 0.0;
    currentSize = 0;
    /** why this one is not working **/
//    vector<Node*> bucket(bucketCount, new Node());
    for (int i = 0; i < bucketCount; ++i) ///push a linklist object to the bucket.
        bucket.push_back(new Node());
}

UnorderedMap::~UnorderedMap() ///destructor
{
    for(int i = 0; i < bucket.size(); i++)
    {
        while (bucket[i])
        {
            Node* temp = bucket[i];
            bucket[i] = bucket[i]->next;
            delete temp;
        }
    }
}

UnorderedMap::Iterator UnorderedMap::begin() const ///find the first node that valid.
{
    int i = 0;
    while(bucket.at(i)->key == "" && i < bucket.size()) i++;
    return Iterator(i, this); /** why just add const in the parameter, everything works fine? important question! **/
}

UnorderedMap::Iterator UnorderedMap::end() const ///find the last bucket's last node's next.
{
    Node* node = bucket.at(bucket.size() - 1);
    while (node && node->key != "") node = node->next;
    return Iterator(node);
}

std::string& UnorderedMap::operator[] (std::string const& key) ///add if key not exist, or view the key's data.
{
    Node* node = bucket[hashFunction(key.c_str(), bucketCount)];
    Node* temp = findNode(key, node);
    if (temp) return temp->name;

    if (node->key == "") node->key = key;
    else addNode(key, "", node);

    currentSize++;
    currLF = (double)currentSize / bucketCount;
    if (currLF >= LF) rehash();
    return node->name;
}

void UnorderedMap::rehash()
{
    bucketCount *= 2;
    vector<Node*> doubleBucket;
    for (int i = 0; i < bucketCount; ++i)
        doubleBucket.push_back(new Node());
    UnorderedMap::Iterator e = end();
    for (auto it = begin(); it != e; ++it) {
        int index = hashFunction(it.node->key.c_str(), bucketCount);
        Node* node = doubleBucket.at(index);
        addNode(it.node->key, it.node->name, node);
    }
    bucket = doubleBucket;

    currLF = (double)size() / bucketCount;
}

void UnorderedMap::remove(std::string const& key)
{
    deleteNode(key, bucket[hashFunction(key.c_str(), bucketCount)]);///delete the node inside the list.
    currentSize--;
    currLF = (double)currentSize / bucketCount; ///renew current load factor.
}

unsigned int UnorderedMap::size()
{
    return currentSize;
}

double UnorderedMap::loadFactor()
{
    return currLF;
}

//implement other operators in Iterator class

//Do not change main()
int main()
{
    int lines = 0, buckets = 0;
    double maxLoadFactor = 0.0;
    std::string command = "", ufid = "", name = "", key = "";
    std::cin >> lines >> buckets >> maxLoadFactor;
    UnorderedMap myMap = UnorderedMap(buckets, maxLoadFactor);
    while(lines--)
    {
        std::cin >> command;
        if(command == "hash")
        {
            std::cin >> key;
            const char* convertedKey = key.c_str();
            std::cout << hashFunction(convertedKey, buckets) << "\n";
        }
        else if(command == "insert")
        {
            std::cin >> ufid >> name;
            myMap[ufid] = name;
        }
        else if(command == "size")
        {
            std::cout << myMap.size() <<"\n";
        }
        else if(command == "load")
        {
            std::cout << std::fixed << std::setprecision(2) << myMap.loadFactor() <<"\n";
        }
        else if(command == "search")
        {
            std::cin >> ufid;
            std::cout << myMap[ufid] << "\n";
        }
        else if(command == "traverse")
        {
            for (UnorderedMap::Iterator iter = myMap.begin(); iter != myMap.end(); ++iter)
            {
                std::cout << (*iter).first << " " << (*iter).second << "\n";
            }

            /* This should also work
                for (auto tableEntry: myMap)
                {
                    std::cout << tableEntry.first << " " << tableEntry.second << "\n";
                }
            */
        }
        else if(command == "remove")
        {
            std::cin >> ufid;
            myMap.remove(ufid);
        }
    }

    return 0;
}
