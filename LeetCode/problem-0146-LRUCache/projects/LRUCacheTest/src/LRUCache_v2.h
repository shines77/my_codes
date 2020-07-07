
#ifndef LEETCODE_LRUCACHE_V2_H
#define LEETCODE_LRUCACHE_V2_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>

#include <list>
#include <map>
#include <unordered_map>
#include <utility>

//
// Problem title: 146. LRU Cache
// Problem URL: https://leetcode.com/problems/lru-cache/
//

namespace LeetCode {
namespace V2 {

struct DoubleLinkedList {
	DoubleLinkedList * prev;
	DoubleLinkedList * next;
	int key;
	int value;

	DoubleLinkedList(int key, int value)
        : key(key), value(value), prev(NULL), next(NULL) {
    }
};

class LRUCache {
private:
	// 容量
	int capacity_;
	// 表头
	DoubleLinkedList * head_;
	// 表尾
	DoubleLinkedList * tail_;
	// 缓存，记录key所对应的链表节点
	std::unordered_map<int, DoubleLinkedList *> cache_;

public:
	LRUCache(int capacity) : capacity_(0), head_(nullptr), tail_(nullptr) {
		// 初始化
		capacity_ = capacity;
		head_ = new DoubleLinkedList(-1, -1);
		tail_ = new DoubleLinkedList(-1, -1);
		head_->next = tail_;
		tail_->prev = head_;
	}

	// 表尾插入结点
	void pushNode(DoubleLinkedList * node) {
		node->prev = tail_->prev;
		tail_->prev->next = node;
		node->next = tail_;
		tail_->prev = node;
	}

	// 从链表中移除节点
	void removeNode(DoubleLinkedList * node) {
		// 将该节点从链表中移除
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}

	int get(int key) {
		// 如果缓存中查找到该key, 则需更新该key为最近使用（即放到表尾）
        auto iter = cache_.find(key);
		if (iter != cache_.end()) {
			DoubleLinkedList * node = iter->second;
			// 从链表中移除该节点
			removeNode(node);
			// 将该节点放至表尾
			pushNode(node);
			return node->value;
		}
		return -1;
	}

	void put(int key, int value) {
		// 如果缓存中有当前key
        auto iter = cache_.find(key);
		if (iter != cache_.end()) {
			DoubleLinkedList * node = iter->second;
			// 从链表中移除该节点
			removeNode(node);
			// 将该节点放至表尾
			pushNode(node);
			// 修改节点对应的值
			node->value = value;
		}
        else {
		    // 如果内存已经放满
		    if (cache_.size() >= capacity_) {
			    // 获取链表头结点的key
			    int topKey = head_->next->key;
			    // 移除链表中的首个节点，并从memory中移除该key
			    removeNode(head_->next);
                DoubleLinkedList * topNode = head_->next;
                //if (topNode != nullptr)
                //    delete topNode;
			    cache_.erase(topKey);
		    }

		    // 表尾放置新key
		    DoubleLinkedList * node = new DoubleLinkedList(key, value);
		    pushNode(node);

		    // 存入缓存
		    cache_[key] = node;
        }
	}
};

} // namespace V2
} // namespace LeetCode

#endif // LEETCODE_LRUCACHE_V2_H
