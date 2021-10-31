#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>

using namespace std;

class linkList {
public:
    struct Node {
        string key;
        string name;
        Node *next;
        Node *prev;

        Node(string key, string name) {
            this->name = name;
            this->key = key;
            prev = nullptr;
            next = nullptr;
        };

        Node &operator=(const Node &node) {
            this->key = node.key;
            this->name = node.name;
            this->next = node.next;
            this->prev = node.prev;
            return *this;
        }
    };

private:
    Node *head;
    Node *tail;
    int listSize;

public:
    linkList() {
        head = nullptr;
        tail = nullptr;
        listSize = 0;
    }

    ~linkList() {
        Node *temp;
        while (head) {
            temp = head;
            head = head->next;
            delete temp;
        }
    }

    void setHead(string key, string name) {
        head = new Node(key, name);
        listSize++;
        tail = head;
    }

    void addTail(string key, string name) {
        if (!head)
            setHead(key, name);
        else {
            Node* temp = tail;
            tail->next = new Node(key, name);
            tail = tail->next;
            tail->prev = temp;
            listSize++;
        }
    }

    Node *findNode(string key)
    {
        Node* temp = head;
        while (temp && temp->key != key)
            temp = temp->next;
        return temp? : nullptr;
    }

    void deleteNode(string key)
    {
        Node* node = findNode(key);
        if (head->next || tail->prev)
        {
            if (node == head)
            {
                head = head->next;
                head->prev = nullptr;
            }else if(node == tail)
            {
                tail = tail->prev;
                tail->next = nullptr;
            } else
            {
                node->prev->next = node->next;
                node->next->prev = node->prev;
            }
        }
        delete node;
        if (head == node) head = nullptr;
        if (tail == node) tail = nullptr;
        node = NULL;
        this->listSize--;
    }

    Node* getHead() {return head;}
    Node* getTail() {return tail;}
    int getSize() {return listSize;}
};

unsigned int hashFunction(char const* key, int table_size)
{
    int b;
    int count = strlen(key);
    unsigned int hashCode = 0;
    for (int i = 0; i < count; i++)
    {
        if (i % 2 == 0) //even case
        {
            b = (hashCode << 7) ^ key[i] ^ (hashCode >> 3);
        } else //odd case
        {
            b = (hashCode << 11) ^ key[i] ^ (hashCode >> 5);
            b = ~b;
        }
        hashCode = hashCode ^ b;
    }
    hashCode = hashCode | (1 << 32);
    return hashCode % table_size;
}

class UnorderedMap
{
private:
    //define your data structure here
    vector<linkList*> *bucket;
    //define other attributes e.g. bucket count, maximum load factor, size of table, etc.
    unsigned int bucketCount;
    double LF;
    double currLF;

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

    class Iterator
    {
        linkList* list;
        linkList::Node* node;
        int currentBucket;
        const UnorderedMap* map;
    public:
        //this constructor does not need to be a default constructor;
        //the parameters for this constructor are up to your discretion.
        //hint: you may need to pass in an UnorderedMap object.
        Iterator(int currentBucket, const UnorderedMap* map)
        {
            this->list = map->bucket->at(currentBucket);
            this->node = list->getHead();
            this->map = map;
            this->currentBucket = currentBucket;
        }
        Iterator(linkList::Node* node)
        {
            this->list = nullptr;
            this->node = node;
            this->map = nullptr;
            this->currentBucket = 0;
        }
        Iterator& operator=(Iterator const& rhs) {this->node = rhs.node;}
        Iterator& operator++()
        {
            if (node)
                node = node->next;

            while (!node && ++currentBucket < map->bucket->size())
            {
                list = map->bucket->at(currentBucket);
                node = list->getHead();
            }
            return *this;
        }
        bool operator!=(Iterator const& rhs) {return rhs.node != this->node;}
        bool operator==(Iterator const& rhs) {return rhs.node == this->node;}
        std::pair<std::string, std::string> operator*() const {return make_pair(node->key, node->name);}
        friend class UnorderedMap;
    };
};

UnorderedMap::UnorderedMap(unsigned int bucketCount, double loadFactor)
{
    this->bucketCount = bucketCount;
    LF = loadFactor;
    currLF = 0.0;
    /** why this one is not working **/
//    bucket = new vector<linkList*>(bucketCount, new linkList());
    bucket = new vector<linkList*>;
    for (int i = 0; i < bucketCount; ++i)
        bucket->push_back(new linkList());
}

UnorderedMap::~UnorderedMap()
{
    for(int i = 0; i < bucket->size(); i++)
        delete bucket->at(i);
    delete bucket;
}

UnorderedMap::Iterator UnorderedMap::begin() const
{
    int i = 0;
    while(bucket->at(i)->getSize() == 0 && i < bucket->size()) i++;
    return Iterator(i, this); /** why just add const in the parameter, everything works fine? important question! **/
}

UnorderedMap::Iterator UnorderedMap::end() const
{
    linkList::Node* node = bucket->at(bucket->size() - 1)->getTail();
    if (node) node = node->next;
    return Iterator(node);
}

std::string& UnorderedMap::operator[] (std::string const& key)
{
    linkList* list = bucket->at(hashFunction(key.c_str(), bucketCount));
    linkList::Node* node = list->findNode(key);
    if (node)
        return node->name;

    list->addTail(key, "");
    currLF = (double)size() / bucketCount;
    if (currLF >= LF) rehash();
    return list->findNode(key)->name;
}

void UnorderedMap::rehash()
{
    bucketCount *= 2;
    vector<linkList*> *doubleBucket =  new vector<linkList*>;
    for (int i = 0; i < bucketCount; ++i)
        doubleBucket->push_back(new linkList());
    UnorderedMap::Iterator e = end();
    for (UnorderedMap::Iterator it = begin(); it != e; ++it) /** why it++ not work here **/
        doubleBucket->at(hashFunction(it.node->key.c_str(), bucketCount))->addTail(it.node->key, it.node->name);
    vector<linkList*> *temp = bucket;
    bucket = doubleBucket;

    delete temp;

    currLF = (double)size() / bucketCount;
}

void UnorderedMap::remove(std::string const& key)
{
    linkList* list = bucket->at(hashFunction(key.c_str(), bucketCount));
    list->deleteNode(key);
    currLF = (double)size() / bucketCount;
}

unsigned int UnorderedMap::size()
{
    int count = 0;
    for (int i = 0; i < bucketCount; i++)
        count += bucket->at(i)->getSize();
    return count;
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
