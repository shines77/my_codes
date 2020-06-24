
//
// See: https://leetcode.com/submissions/detail/26838843/
//
// Runtime: 70 ms
//

#include <assert.h>

struct LRUNode {
    int         key;
    int         value;
    LRUNode *   prev;
    LRUNode *   next;
};

struct HashNode {
    int         key;
    LRUNode *   value;
};

class HashTable {
private:
    int         mSize;
    int         mMask;
    HashNode *  mTableA;
    HashNode *  mTableB;

    static const unsigned int kMaxQueryStep = 6;

public:
    HashTable(int capacity) {
        mSize = calcSize(capacity);
        mMask = mSize - 1;
        mTableA = new HashNode[mSize];
        mTableB = new HashNode[mSize];
        ::memset(mTableA, -1, sizeof(HashNode) * mSize);
        ::memset(mTableB, -1, sizeof(HashNode) * mSize);
    }

    ~HashTable() {
        /*
        if (mTableA)
            delete[] mTableA;
        if (mTableB)
            delete[] mTableB;
        //*/
    }

protected:
    int getHashA(int key) { return key & mMask; }
    int getHashB(int key) { return (key ^ 4491738) & mMask; }

    int calcSize(int capacity) {
        int size = 16;
        if (capacity > 16) {
            while (size < capacity)
                size <<= 1;
            if ((size - capacity) < size / 4)
                size = size << 1;
        }
        else size = 32;
        return size;
    }

public:
    void add(int key, LRUNode * node) {
        int indexA = getHashA(key);

        HashNode * startPtr = &mTableA[indexA];
        HashNode * endPtr   = startPtr + kMaxQueryStep;
        do {
            if (startPtr->key == -1) {
                startPtr->key   = key;
                startPtr->value = node;
                return;
            }
            if (startPtr->key == key) {
                startPtr->value = node;
                return;
            }
            startPtr++;
        } while (startPtr != endPtr);

        int indexB = getHashB(key);
        int startIndexB = indexB;
        do {
            if (mTableB[indexB].key >= 0 && mTableB[indexB].key != key) {
                indexB = (indexB + 1) & mMask;
                if (indexB == startIndexB)
                    break;
            }
            else {
                mTableB[indexB].key = key;
                mTableB[indexB].value = node;;
                return;
            }
        } while (1);
    }

    LRUNode * find(int key) {
        int indexA = getHashA(key);

        HashNode * startPtr = &mTableA[indexA];
        HashNode * endPtr   = startPtr + kMaxQueryStep;
        do {
            if (startPtr->key == key)
                return startPtr->value;
            if (startPtr->key == -1)
                break;
            startPtr++;
        } while (startPtr != endPtr);

        int indexB = getHashB(key);
        int startIndexB = indexB;
        do {
            if (mTableB[indexB].key != key) {
                if (mTableB[indexB].key != -1) {
                    indexB = (indexB + 1) & mMask;
                    if (indexB == startIndexB)
                        break;
                }
                else {
                    return NULL;
                }
            }
            else return mTableB[indexB].value;
        } while (1);

        return NULL;
    }

    void remove(int key) {
        int indexA = getHashA(key);

        HashNode * startPtr = &mTableA[indexA];
        HashNode * endPtr   = startPtr + kMaxQueryStep;
        do {
            if (startPtr->key == key) {
                startPtr->key = -2;
                return;
            }
            if (startPtr->key == -1)
                break;
            startPtr++;
        } while (startPtr != endPtr);

        int indexB = getHashB(key);
        int startIndexB = indexB;
        do {
            if (mTableB[indexB].key != key) {
                indexB = (indexB + 1) & mMask;
                if (indexB == startIndexB)
                    break;
            }
            else {
                mTableB[indexB].key = -2;
                return;
            }
        } while (1);
    }
};

class LRUCache {
private:
    int         mSize;
    int         mCapacity;
    LRUNode *   mCacheList;
    LRUNode *   mCacheListLast;
    LRUNode *   mHeadNode;
    LRUNode *   mTailNode;
    HashTable   mHashTable;

public:
    LRUCache(int capacity)
        : mSize(0), mCapacity(capacity), mHashTable(capacity) {
        mCacheList = new LRUNode[capacity];
        mCacheListLast = mCacheList;

        mHeadNode = NULL;
        mTailNode = NULL;
    }

    ~LRUCache() {
        /*
        if (mCacheList)
            delete[] mCacheList;
        //*/
    }

    int getSize() { return mSize; }

    void appendNode(int key, int value) {
        LRUNode * newNode = mCacheListLast;
        mCacheListLast++;
        mSize++;

        assert(mSize <= mCapacity);

        newNode->key   = key;
        newNode->value = value;
        newNode->prev  = NULL;
        newNode->next  = mHeadNode;

        // Add a key
        mHashTable.add(key, newNode);

        if (mHeadNode != NULL) {
            // Record the head node (the recent used node).
            mHeadNode->prev = newNode;
            mHeadNode = newNode;
        }
        else {
            // If mHeadNode is null, mTailNode must be null.
            assert(mTailNode == NULL);
            // Record the head and tail node (the recent used node and the last used node).
            mHeadNode = mTailNode = newNode;
        }
    }

    void eliminateNode(int key, int value) {
        LRUNode * oldTailNode = mTailNode;
        if (key != oldTailNode->key) {
            // Remove a key
            mHashTable.remove(oldTailNode->key);
            // Add a key
            mHashTable.add(key, oldTailNode);

            oldTailNode->key = key;
        }
        oldTailNode->value = value;

        LRUNode * newTailNode = oldTailNode->prev;
        if (newTailNode) {
            // Record the new tail node (the last used node).
            newTailNode->next = NULL;
            mTailNode = newTailNode;
        }
        oldTailNode->prev = NULL;

        // To avoid their own point to themselves.
        if (oldTailNode != mHeadNode) {
            oldTailNode->next = mHeadNode;

            // Record the head node (the recent used node).
            mHeadNode->prev = oldTailNode;
            mHeadNode = oldTailNode;
        }
        else {
            oldTailNode->next = NULL;
        }
    }

    void pickupNode(LRUNode * newNode, int value) {
        // Modify the value directly.
        newNode->value = value;

        if (newNode != mHeadNode) {
            if (newNode != mTailNode) {
                // It's not head or tail node.
                newNode->prev->next = newNode->next;
                newNode->next->prev = newNode->prev;
            }
            else {
                // It's the tail node, record the new tail node (the last used node).
                newNode->prev->next = NULL;
                mTailNode = newNode->prev;
            }

            // Insert new head node to the header and
            // record the recent used node (the head node).
            newNode->prev = NULL;
            newNode->next = mHeadNode;

            mHeadNode->prev = newNode;
            mHeadNode = newNode;
        }
    }

    LRUNode * find(int key) {
        return mHashTable.find(key);
    }

    int get(int key) {
        LRUNode * node = find(key);
        if (node) {
            pickupNode(node, node->value);
            return node->value;
        }
        return -1;
    }

    void set(int key, int value) {
        LRUNode * node = find(key);
        if (node == NULL) {
            // It's a new key.
            if (mSize < mCapacity) {
                // The cache list capacity is not full, append a new node to head only.
                appendNode(key, value);
            }
            else {
                // The cache list capacity is full, must be eliminate a node and pickup it to head.
                eliminateNode(key, value);
            }
        }
        else {
            // Pickup the node to head.
            pickupNode(node, value);
        }
    }
};
