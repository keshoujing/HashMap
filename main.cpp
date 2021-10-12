#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>
#include <list>

using namespace std;

template<typename KEY, typename VALUE>
class node
{
public:
    KEY _key;
    VALUE _value;
    node* next;
    node* pre;
    node(string key, string value)
    {
        _key = key;
        _value = value;
        next = nullptr;
        pre = nullptr;
    };
    node& operator=(const node& node)
    {
        _key = node._key;
        _value = node._value;
        next = node.next;
        pre = node.pre;
        return *this;
    }
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
    return hashCode;
}

class UnorderedMap
{
private:
    //define your data structure here
    node<string, string>** _node;

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
          node<string, string>** table;
    public:
        //this constructor does not need to be a default constructor;
        //the parameters for this constructor are up to your discretion.
        //hint: you may need to pass in an UnorderedMap object.
        Iterator(node<string, string>** table) {this->table = table;}

        Iterator& operator=(Iterator const& rhs)
        {

        }
        Iterator& operator++()
        {

        }
        bool operator!=(Iterator const& rhs) {}
        bool operator==(Iterator const& rhs) {}
        std::pair<std::string, std::string> operator*() const {}
        friend class UnorderedMap;
    };
};

UnorderedMap::UnorderedMap(unsigned int bucketCount, double loadFactor)
{
    this->bucketCount = bucketCount;
    LF = loadFactor;
    table = new node* [bucketCount];
    for (int i = 0; i < bucketCount; ++i)
    {
        table[i] = nullptr;
    }
}

UnorderedMap::~UnorderedMap()
{
    for (int i = 0; i < bucketCount; ++i)
    {
        delete[] table[i];
    }
}

UnorderedMap::Iterator UnorderedMap::begin() const
{
    Iterator(this).getFirst();

}

UnorderedMap::Iterator UnorderedMap::end() const
{

}

std::string& UnorderedMap::operator[] (std::string const& key)
{

}

void UnorderedMap::rehash()
{

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