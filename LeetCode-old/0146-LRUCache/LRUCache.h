
struct LRUItem {
    int                 key;
    int                 value;
    struct LRUItem *    prev;
    struct LRUItem *    next;
};

struct HashItem {
    int                 key;
    struct LRUItem *    value;
};

int                 mHashSize;
int                 mMask;
struct HashItem *   mTableA;
struct HashItem *   mTableB;

static const unsigned int kMaxQueryStep = 5;

int getHashA(int key) { return key & mMask; }
int getHashB(int key) { return (key ^ 4491738) & mMask; }

int hashTableCalcSize(int capacity) {
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

void hashTableInit(int capacity) {
    mHashSize = hashTableCalcSize(capacity);
    mMask = mHashSize - 1;
    mTableA = malloc(sizeof(struct HashItem) * mHashSize);
    mTableB = malloc(sizeof(struct HashItem) * mHashSize);
    memset(mTableA, -1, sizeof(struct HashItem) * mHashSize);
    memset(mTableB, -1, sizeof(struct HashItem) * mHashSize);
}

void hashTableFree() {
    /*
    if (mTableA)
        free(mTableA);
    if (mTableB)
        free(mTableB);
    //*/
}

void hashTableAdd(int key, struct LRUItem * item) {
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

struct LRUItem * hashTableFind(int key) {
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

void hashTableRemove(int key) {
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

int                 mCacheSize;
int                 mCapacity;
struct LRUItem *    mCacheList;
struct LRUItem *    mCacheListLast;
struct LRUItem *    mHeadItem;
struct LRUItem *    mTailItem;
    
void lruCacheInit(int capacity) {
    mCacheSize = 0;
    mCapacity  = capacity;
    
    mCacheList = malloc(sizeof(struct LRUItem) * capacity);
    mCacheListLast = mCacheList;

    mHeadItem = NULL;
    mTailItem = NULL;
    
    hashTableInit(capacity);
}

void lruCacheFree() {
    /*
    if (mCacheList)
        free(mCacheList);
    hashTableFree();
    //*/
}

void appendItem(int key, int value) {
    struct LRUItem * newItem = mCacheListLast;
    mCacheListLast++;
    mCacheSize++;

    newItem->key   = key;
    newItem->value = value;
    newItem->prev  = NULL;
    newItem->next  = mHeadItem;

    // Add a key
    hashTableAdd(key, newItem);

    if (mHeadItem != NULL) {
        // Record the recent used item.
        mHeadItem->prev = newItem;
        mHeadItem = newItem;
    }
    else {
        // Record the recent used item and the last used item.
        mHeadItem = mTailItem = newItem;
    }
}

void eliminateItem(int key, int value) {
    struct LRUItem * tailItem = mTailItem;
    if (key != tailItem->key) {
        // Remove a key
        hashTableRemove(tailItem->key);
        // Add a key
        hashTableAdd(key, tailItem);

        tailItem->key = key;
    }
    tailItem->value = value;

    struct LRUItem * newTailItem = tailItem->prev;
    if (newTailItem) {
        newTailItem->next = NULL;
        // Record the last used item
        mTailItem = newTailItem;
    }
    tailItem->prev = NULL;

    // To avoid their own point to themselves.
    if (tailItem != mHeadItem) {
        tailItem->next = mHeadItem;

        // Record the recent used item
        mHeadItem->prev = tailItem;
        mHeadItem = tailItem;
    }
    else tailItem->next = NULL;
}

void pickupItem(struct LRUItem * cacheItem, int value) {
    // Modify the value directly.
    cacheItem->value = value;

    if (cacheItem != mHeadItem) {
        if (cacheItem != mTailItem) {
            // It's not head and not tail.
            cacheItem->prev->next = cacheItem->next;
            cacheItem->next->prev = cacheItem->prev;
        }
        else {
            // Record the last used item.
            cacheItem->prev->next = NULL;
            mTailItem = cacheItem->prev;
        }

        // Link the old head item to new head item.
        cacheItem->prev = NULL;
        cacheItem->next = mHeadItem;

        // Modify the recent used item.
        mHeadItem->prev = cacheItem;
        // Record the recent used item.
        mHeadItem = cacheItem;
    }
}

struct LRUItem * lruCacheFind(int key) {
    return hashTableFind(key);
}

int lruCacheGet(int key) {
    struct LRUItem * foundItem = lruCacheFind(key);
    if (foundItem) {
        pickupItem(foundItem, foundItem->value);
        return foundItem->value;
    }
    return -1;
}
    
void lruCacheSet(int key, int value) {
    struct LRUItem * foundItem = lruCacheFind(key);
    if (foundItem == NULL) {
        // It's a new key.
        if (mCacheSize < mCapacity) {
            // The cache list capacity is not full, append a new item to head only.
            appendItem(key, value);
        }
        else {
            // The cache list capacity is full, must be eliminate a item.
            eliminateItem(key, value);
        }
    }
    else {
        // Pickup the item to head.
        pickupItem(foundItem, value);
    }
}
