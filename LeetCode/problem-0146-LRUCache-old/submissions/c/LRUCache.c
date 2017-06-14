//
// See: https://leetcode.com/submissions/detail/26803094/
//
// Runtime: 28 ms
//

#include <assert.h>

struct LRUItem {
    int              key;
    int              value;
    struct LRUItem * prev;
    struct LRUItem * next;
};

struct HashItem {
    int              key;
    struct LRUItem * value;
};

int                 mHashSize;
int                 mMask;
struct HashItem *   mTableA;
struct HashItem *   mTableB;

static const unsigned int kMaxQueryStep = 5;

int getHashA(int key) { return key & mMask; }
int getHashB(int key) { return (key ^ 4491738) & mMask; }

int HashTableCalcSize(int capacity) {
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

void HashTableInit(int capacity) {
    mHashSize = HashTableCalcSize(capacity);
    mMask = mHashSize - 1;
    mTableA = malloc(sizeof(struct HashItem) * mHashSize);
    mTableB = malloc(sizeof(struct HashItem) * mHashSize);
    memset(mTableA, -1, sizeof(struct HashItem) * mHashSize);
    memset(mTableB, -1, sizeof(struct HashItem) * mHashSize);
}

void HashTableFree() {
    /*
    if (mTableA)
        free(mTableA);
    if (mTableB)
        free(mTableB);
    //*/
}

void HashTableAdd(int key, struct LRUItem * item) {
    int indexA = getHashA(key);

    struct HashItem * startPtr = &mTableA[indexA];
    struct HashItem * endPtr   = startPtr + kMaxQueryStep;
    do {
        if (startPtr->key == -1) {
            startPtr->key   = key;
            startPtr->value = item;
            return;
        }
        if (startPtr->key == key) {
            startPtr->value = item;
            return;
        }
        startPtr++;
    } while (startPtr != endPtr);

    int indexB = getHashB(key);
    int startIndex = indexB;
    do {
        if (mTableB[indexB].key >= 0 && mTableB[indexB].key != key) {
            indexB = (indexB + 1) & mMask;
            if (indexB != startIndex) continue;
        }
        else {
            mTableB[indexB].key = key;
            mTableB[indexB].value = item;;
            return;
        }
    } while (1);
}

struct LRUItem * HashTableFind(int key) {
    int indexA = getHashA(key);

    struct HashItem * startPtr = &mTableA[indexA];
    struct HashItem * endPtr   = startPtr + kMaxQueryStep;
    do {
        if (startPtr->key == key)
            return startPtr->value;
        if (startPtr->key == -1)
            break;
        startPtr++;
    } while (startPtr != endPtr);

    int indexB = getHashB(key);
    int startIndex = indexB;
    do {
        if (mTableB[indexB].key != key) {
            if (mTableB[indexB].key != -1) {
                indexB = (indexB + 1) & mMask;
                if (indexB != startIndex) continue;
            }
            else return NULL;
        }
        else return mTableB[indexB].value;
    } while (1);

    return NULL;
}

void HashTableRemove(int key) {
    int indexA = getHashA(key);

    struct HashItem * startPtr = &mTableA[indexA];
    struct HashItem * endPtr   = startPtr + kMaxQueryStep;
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
    int startIndex = indexB;
    do {
        if (mTableB[indexB].key != key) {
            indexB = (indexB + 1) & mMask;
            if (indexB != startIndex) continue;
        }
        else {
            mTableB[indexB].key = -2;
            return;
        }
    } while (1);
}

int              mCacheSize = 0;
int              mCapacity = 0;
struct LRUItem * mCacheList = NULL;
struct LRUItem * mCacheListLast = NULL;
struct LRUItem * mHeadItem = NULL;
struct LRUItem * mTailItem = NULL;

void LRUCacheInit(int capacity) {
    mCacheSize = 0;
    mCapacity  = capacity;

    mCacheList = malloc(sizeof(struct LRUItem) * capacity);
    mCacheListLast = mCacheList;

    mHeadItem = NULL;
    mTailItem = NULL;

    HashTableInit(capacity);
}

void LRUCacheFree() {
    /*
    if (mCacheList)
        free(mCacheList);
    HashTableFree();
    //*/
}

void appendItem(int key, int value) {
    struct LRUItem * newItem = mCacheListLast;
    mCacheListLast++;
    mCacheSize++;

    assert(mCacheSize <= mCapacity);

    newItem->key   = key;
    newItem->value = value;
    newItem->prev  = NULL;
    newItem->next  = mHeadItem;

    // Add a key
    HashTableAdd(key, newItem);

    if (mHeadItem != NULL) {
        // Record the head item (the recent used item).
        mHeadItem->prev = newItem;
        mHeadItem = newItem;
    }
    else {
        // If mHeadItem is null, mTailItem must be null.
        assert(mTailItem == NULL);
        // Record the head and tail item (the recent used item and the last used item).
        mHeadItem = mTailItem = newItem;
    }
}

void eliminateItem(int key, int value) {
    struct LRUItem * oldTailItem = mTailItem;
    if (key != oldTailItem->key) {
        // Remove a key
        HashTableRemove(oldTailItem->key);
        // Add a key
        HashTableAdd(key, oldTailItem);

        oldTailItem->key = key;
    }
    oldTailItem->value = value;

    struct LRUItem * newTailItem = oldTailItem->prev;
    if (newTailItem) {
        // Record the new tail item (the last used item).
        newTailItem->next = NULL;
        mTailItem = newTailItem;
    }
    oldTailItem->prev = NULL;

    // To avoid their own point to themselves.
    if (oldTailItem != mHeadItem) {
        oldTailItem->next = mHeadItem;

        // Record the head item (the recent used item).
        mHeadItem->prev = oldTailItem;
        mHeadItem = oldTailItem;
    }
    else {
        oldTailItem->next = NULL;
    }
}

void pickupItem(struct LRUItem * newItem, int value) {
    // Modify the value directly.
    newItem->value = value;

    if (newItem != mHeadItem) {
        if (newItem != mTailItem) {
            // It's not head or tail item.
            newItem->prev->next = newItem->next;
            newItem->next->prev = newItem->prev;
        }
        else {
            // It's the tail item, record the new tail item (the last used item).
            newItem->prev->next = NULL;
            mTailItem = newItem->prev;
        }

        // Insert new head item to the header and
        // record the recent used item (the head item).
        newItem->prev = NULL;
        newItem->next = mHeadItem;

        mHeadItem->prev = newItem;
        mHeadItem = newItem;
    }
}

struct LRUItem * LRUCacheFind(int key) {
    return HashTableFind(key);
}

int LRUCacheGet(int key) {
    struct LRUItem * item = LRUCacheFind(key);
    if (item) {
        pickupItem(item, item->value);
        return item->value;
    }
    return -1;
}

void LRUCacheSet(int key, int value) {
    struct LRUItem * item = LRUCacheFind(key);
    if (item == NULL) {
        // It's a new key.
        if (mCacheSize < mCapacity) {
            // The cache list capacity is not full, append a new item to head only.
            appendItem(key, value);
        }
        else {
            // The cache list capacity is full, must be eliminate a item and pickup it to head.
            eliminateItem(key, value);
        }
    }
    else {
        // Pickup the item to head.
        pickupItem(item, value);
    }
}
