#pragma once


namespace utils
{
    //Simple pair struct
    template<typename K, typename V>
    struct Pair
    {
        K first;
        V second;
    };

	//this vector is copied from my OWN stl inside of my OWN game engine.
	// It can be found here: https://github.com/thatalloguy/Catos/blob/main/Runtime/src/stl/vector.h
	// I wrote it back in the summer of 2024 to get a better grasp at C++
    template<typename T>
    class vector {

    public:

        /// Initializes the vector to empty.
        vector() : buf(nullptr), size(0), maxSize(0) {};



        template<typename ...Args>
        vector(Args ... args) {
            (push_back(args), ...);
        }

        /// Move Constructor
        vector(vector&& obj) {
            buf = obj.buf;
            size = obj.size;
            maxSize = obj.size;

        }



        ~vector() {
            printf("deleting buf\n");
            delete[] buf;
        }

        /// Copies the obj.
        vector& operator=(const vector& obj) {
            //if (*this == obj) return *this; // no self asignment.

            if (obj.size >= maxSize)
                reserve(maxSize + 8);


            for (int i = 0; i < obj.size; i++)
                buf[i] = obj.buf[i];

            return *this;
        }


        /// allocates enough memory for the amount given.
        void reserve(int amount) {

            // no need if its the smaller or same size.
            if (amount < maxSize || amount < 0) return;

            if (buf == nullptr || maxSize == 0) {
                buf = new T[amount];
                maxSize = amount;
                return;
            }

            //allocate a new buffer with the desired size.
            T* temp = new T[amount];

            // copy our current items to that array.
            for (int i = 0; i < size; i++)
                temp[i] = buf[i];


            //destroy our previous allocated memory;
            delete[] buf;

            // Now our buffer should point to the newly allocated memory.
            buf = temp;
            maxSize = amount;
        }

        void resize(unsigned int newSize) {

            // no need if its the smaller or same size.
            if (newSize == maxSize) return;

            //if (newSize > size) throw out_of_range();


            //allocate a new buffer with the desired size.
            T* temp = new T[newSize];

            // copy our current items to that array.
            for (int i = 0; i < size; i++)
                temp[i] = buf[i];


            //destroy our previous allocated memory;
            delete[] buf;

            // Now our buffer should point to the newly allocated memory.
            buf = temp;
            maxSize = newSize;
        }

        /// Adds the new item to the array.
        void push_back(T obj) {

            if (size >= maxSize) {
                // do 8 so that we dont have to allocate every push_back.
                reserve(maxSize + 8);
            }

            //add the object to the internal array
            buf[size] = obj;

            //increase the size;
            size++;

        }

        /// clears the buffer
        void clear() {
            size = 0;
        };

        /// removes the item at the index
        void remove(int index) {

            for (int i = index; i < size - 1; i++) {
                buf[i] = buf[i + 1];
            }

            size--;
        };

        /// returns the item at the desired index.
        T& at(int index) {
            return buf[index];
        }

        /// gives the last item.
        T& back() {
            return buf[size - 1];
        }

        ///Removes the last item
        void pop_back() {
            remove(size - 1);
        }

        /// gives the internal buffer
        T* data() { return buf; };


        const T* data() const { return buf; };

        /// returns the amount of objects in the internal buffer.
        int length() const {
            return size;
        }

        ///Returns if the size is <= 0
        bool empty() const {
            return size <= 0;
        }

        /// returns the maximum amount of objects in our internal buffer
        int maxLength() const  {
            return maxSize;
        }

        ///  Returns the obj for the given index.
        T& operator[](int i) {
            if (i > size && i < maxSize)
                size = i;

            return buf[i];
        }

        void setLength(int length) {
            size = length;
        }



        /// Deletes the allocated objects.



        /* --- Iterator stuff */

        class iterator;

        iterator begin();

        iterator begin() const;

        iterator end();

        iterator end() const;

        iterator cbegin() const;

        iterator cend() const;




    private:


        T* buf = nullptr;
        // The actual amount of items in the buffer
        int size = 0;
        // The max amount of items in the buffer.
        int maxSize = 0;
    };



    template<class T> class vector<T>::iterator
    {
    public:
        iterator(T* p)
            :_curr(p)
        {
        }

        iterator& operator++()
        {
            _curr++;
            return *this;
        }

        iterator& operator--()
        {
            _curr--;
            return *this;
        }

        T& operator*()
        {
            return *_curr;
        }

        bool operator==(const iterator& b) const
        {
            return *_curr == *b._curr;
        }

        bool operator!=(const iterator& b) const
        {
            return _curr != b._curr;
        }

    private:
        T* _curr;





    };


    template<class T>
    inline typename vector<T>::iterator vector<T>::begin()
    {
        return vector<T>::iterator(&buf[0]);
    };

    template<class T>
    inline typename vector<T>::iterator vector<T>::begin() const {
        return vector<T>::iterator(&buf[0]);
    }

    template<class T>
    inline typename vector<T>::iterator vector<T>::end()
    {
        return vector<T>::iterator(&buf[size]);
    }

    template<class T>
    inline typename vector<T>::iterator vector<T>::end() const {
        return vector<T>::iterator(&buf[size]);
    }

    template<class T>
    inline typename  vector<T>::iterator vector<T>::cbegin() const {
        return vector<T>::iterator(&buf[0]);
    }

    template<class T>
    inline typename vector<T>::iterator vector<T>::cend() const {
        return vector<T>::iterator(&buf[size]);
    }


    struct no_item_found {
        const char* what() {
            return "No Item found with that key\n";
        }
    };

    struct key_not_found {
        const char* what() {
            return "No Bucket found with that key\n";
        }
    };




    template<typename K, typename V>
    class hashnode {
    public:

        hashnode(const K& key, const V& value) : key(key), value(value) {}


        K getKey() const {
            return key;
        }

        V getValue() const {
            return value;
        }

        hashnode* getNext() const {
            return next;
        }

        void setValue(V newVal) {
            value = newVal;
        }

        void setNext(hashnode* newNext) {
            next = newNext;
        }

    private:
        K key;
        V value;
        hashnode* next = nullptr;

    };

    template<typename K>
    struct HashFunc {
        size_t operator()(const K& key, int Size) const {
            return static_cast<size_t>(key) % Size;
        }
    };


    struct StringHashFunc {
        size_t operator()(const std::string& key, int Size) const {
            uint hash = 0;
            for (auto it = key.begin(); it != key.end(); ++it) {
                hash = *it + (hash << 6) + (hash << 16) - hash;
            }
            hash = hash % Size;
            return hash;
        }
    };

    ///------------------------ NOTE ------------
    /// This is my own hashmap implementation copied from:
    /// https://github.com/thatalloguy/SchoolGameApplication/blob/main/Engine/src/stl/hashmap.h

    /**
     * Hashmap structure object.
     * NOTE: alot of custom types such as catos::string dont work.
     * Which has to do with my hashing func. You can provide your own if you really want it.
    **/
    template<typename K, typename V, typename F = HashFunc<K>>
    class hashmap {

    public:

        /// Basic constructor with a startSize of 8
        hashmap(int startSize = 8) : maxSize(startSize) {
            buf = new hashnode<K, V>* [startSize]();
        }

        ~hashmap() {
            cleanup();
        }


        hashnode<K, V>* getStartHashNode() {
            auto entry = buf[0];
            int i = 0;
            while (entry == nullptr && i < maxSize) {
                i++;
                entry = buf[i];
                if (entry != nullptr) {
                    return entry;
                }
            }

            return buf[0];
        };

        /// Gets the value based on the key given.
        V get(const K& key) {

            unsigned int index = static_cast<int>(hashFunc(key, maxSize));
            auto entry = buf[index];

            // loop through all of the buckets with the same hash until we found the right key.
            while (entry != nullptr) {
                if (entry->getKey() == key) {
                    return entry->getValue();
                }
                entry = entry->getNext();
            }

            // could not find item based on given key
            throw no_item_found{};
        }

        bool has(const K& key)
        {

            int index = static_cast<int>(hashFunc(key, maxSize));
            auto entry = buf[index];

            // loop through all of the buckets with the same hash until we found the right key.
            while (entry != nullptr) {
                if (entry->getKey() == key) {
                    return true;
                }
                entry = entry->getNext();
            }

            // could not find item based on given key
            return false;
        }

        /// puts an item in the table.
        void put(const K& key, const V& value) {

            size++;

            if (size >= maxSize) {
                // rehash
                rehash(maxSize + 8);
            }

            int index = static_cast<int>(hashFunc(key, maxSize));
            hashnode<K, V>* prev = nullptr;
            auto entry = buf[index];

            // find an empty bucket that doenst have the same key.
            while (entry != nullptr && entry->getKey() != key) {
                prev = entry;
                entry = entry->getNext();
            }

            if (entry == nullptr) {
                entry = new hashnode<K, V>(key, value);
                if (prev == nullptr) {
                    buf[index] = entry;
                }
                else {
                    prev->setNext(entry);
                }
            }
            else {
                // fallback
                entry->setValue(value);
            }
        }

        /// Removes the item.
        void remove(const K& key) {

            //  should we?
            size--;

            int index = hashFunc(key, maxSize);
            hashnode<K, V>* prev = nullptr;
            auto entry = buf[index];

            // search for the right bucket with the correct key.
            while (entry != nullptr && entry->getKey() != key) {
                prev = entry;
                entry = entry->getNext();
            }

            if (entry == nullptr) {
                throw key_not_found{};
            }
            else {
                if (prev == nullptr) {
                    // remove the first bucket.
                    buf[index] = entry->getNext();
                }
                else {
                    prev->setValue(entry->getNext());
                }

                delete entry;
            }
        }

        /// Creates a new table of the given size and rehash all of the previous items.
        void rehash(int newSize) {

            //First create a new table with the desired size and everything to nullptr's
            hashnode<K, V>** temp = new hashnode<K, V>* [newSize] {nullptr};

            //Loop through all of the old items of the old table.
            for (int i = 0; i < maxSize; i++) {

                auto oldEntry = buf[i];

                // Check if this object is empty, if so we have to rehash the object and its buckets.
                while (oldEntry != nullptr) {

                    // get the new hash
                    int index = static_cast<int>(hashFunc(oldEntry->getKey(), newSize));

                    auto newEntry = temp[index];

                    // If the new location is empty create a new one.
                    if (!newEntry) {
                        temp[index] = new hashnode<K, V>(oldEntry->getKey(), oldEntry->getValue());
                    }
                    else {
                        //if the new location isnt empty search for an empty bucket.
                        auto prev = newEntry->getNext();

                        // get the last bucket
                        while (prev != nullptr) {
                            prev = prev->getNext();
                        }

                        // put our entry in that bucket
                        prev->setNext(new hashnode<K, V>(oldEntry->getKey(), oldEntry->getValue()));

                    }

                    // Get the next one to loop through all child buckets.
                    oldEntry = oldEntry->getNext();
                }
            }
            //Delete the old buffer
            cleanup();

            // switch!
            maxSize = newSize;
            buf = temp;
        }

        int maxLength() { return maxSize; };

        hashnode<K, V>* getNode(int pos) {
            return buf[pos];
        }
    private:

        void cleanup() {
            for (int i = 0; i < maxSize; i++) {

                // get the first bucket for the key / index.
                auto entry = buf[i];

                // loop through all off the buckets with the same key
                while (entry != nullptr) {

                    auto prev = entry;

                    // set the entry to the next bucket of the same bucket.
                    entry = entry->getNext();

                    // delete this bucket
                    delete prev;
                }

                buf[i] = nullptr;
            }

            //destroy the allocated buffer;
            delete[] buf;
        }




        F hashFunc;

        hashnode<K, V>** buf; // just an array of pointers to hashnodes<K, V>
        int size = 0;
        // NOTE everytime the maxSize changes we have REHASH EVERYTHING!!
        int maxSize = 0;

    };


}

