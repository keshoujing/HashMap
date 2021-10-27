#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>
#include <list>

using namespace std;

class linkList
{
public:
    struct Node
    {
        string key;
        string name;
        Node* next;
        Node(string key, string name)
        {
            this->name = name;
            this->key = key;
            next = nullptr;
        };
        Node& operator=(const Node& node)
        {
            this->key = node.key;
            this->name = node.name;
            this->next = node.next;
            return *this;
        }
    };

private:
    Node* head;
    Node* tail;

public:
    linkList(){
        Node* head = nullptr;
        Node* tail = nullptr;
    }

    ~linkList()
    {
        Node* temp;
        while (head)
        {
            temp = head;
            head = head->next;
            delete temp;
        }
    }

    void setHead(string key, string name)
    {
        head = new Node(key, name);
        tail = head;
    }

    void addTail(string key, string name)
    {
        if (!head)
            setHead(key, name);
        else
        {
            tail->next = new Node(key, name);
            tail = tail->next;
        }
    }

    Node* getHead() {return head;}
    Node* getTail() {return tail;}
};

unsigned int hashFunction(char const* key, int table_size)
{
    int b;
    int count = table_size;
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
    return hashCode;
}

class UnorderedMap
{
private:
    //define your data structure here
    vector<linkList*> *bucket;
    //define other attributes e.g. bucket count, maximum load factor, size of table, etc.
    unsigned int bucketCount;
    double LF;

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
        UnorderedMap* map;
    public:
        //this constructor does not need to be a default constructor;
        //the parameters for this constructor are up to your discretion.
        //hint: you may need to pass in an UnorderedMap object.
        Iterator(linkList* list, UnorderedMap* map = nullptr, int currentBucket = 0)
        {
            this->list = list;
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
            if (node->next != nullptr)
                node = node->next;
            else
            {
                if (++currentBucket < map->bucketCount)
                {
                list = map->bucket->at(currentBucket);
                node = list->getHead();
                }
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
    bucket = new vector<linkList*>(bucketCount);
}

UnorderedMap::~UnorderedMap()
{
    for (int i = 0; i < bucketCount; ++i)
        delete bucket->at(i);
}

UnorderedMap::Iterator UnorderedMap::begin() const
{
    return Iterator(bucket->at(0));
}

UnorderedMap::Iterator UnorderedMap::end() const
{
    return Iterator(bucket->at(bucketCount - 1)->getTail()->next);
}

std::string& UnorderedMap::operator[] (std::string const& key)
{

}

void UnorderedMap::rehash()
{
    bucketCount *= 2;
    LF = 0.0;
    vector<linkList*> *doubleBucket =  new vector<linkList*>(bucketCount);
    for (UnorderedMap::Iterator it = begin(); it != end(); ++it) /** why it++ not work here **/
        doubleBucket->at(hashFunction(it.node->key.c_str(), bucketCount))->addTail(it.node->key, it.node->name);
    vector<linkList*> *temp = bucket;
    bucket = doubleBucket;
    delete temp;
}

void UnorderedMap::remove(std::string const& key)
{

}

unsigned int UnorderedMap::size()
{

}

double UnorderedMap::loadFactor()
{

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
