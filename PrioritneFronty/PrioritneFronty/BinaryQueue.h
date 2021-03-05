#pragma once
#include "PriorityQueue.h"
#include <vector>
#include <iostream>

template <typename K, typename T>
class BinaryQueue : public PriorityQueue<K, T>
{
public:
	BinaryQueue();
	~BinaryQueue();
	size_t size() const override;
	PriorityQueueItem<K, T>* push(const K& priority, const T& data) override;
	T pop() override;
	T& peek() override;
	const T peek() const override;
	K peekPriority() override;
private:
	std::vector<PriorityQueueItem<K, T>*>* list_;
	int leftSon(const int index);
	int righSon(const int index);
	int parent(const int index);
	int greaterSon(const int index);
	void heapifyUp(const int index);
	void heapifyDown(const int index);
};



template<typename K, typename T>
inline BinaryQueue<K, T>::BinaryQueue() :
	PriorityQueue<K, T>(),
	list_(new std::vector<PriorityQueueItem<K, T>*>())
{
}

template<typename K, typename T>
inline BinaryQueue<K, T>::~BinaryQueue()
{
	for (PriorityQueueItem<K, T>* item : *this->list_)
	{
		delete item;
	}
	this->list_->clear();
	delete this->list_;
}


template<typename K, typename T>
inline size_t BinaryQueue<K, T>::size() const
{
	return this->list_->size();
}

template<typename K, typename T>
inline PriorityQueueItem<K, T>* BinaryQueue<K, T>::push(const K& priority, const T& data)
{
	PriorityQueueItem<K, T>* node = new PriorityQueueItem<K, T>(priority, data);
	this->list_->push_back(node);
	this->heapifyUp(this->size() - 1);
	return node;
}

template<typename K, typename T>
inline T BinaryQueue<K, T>::pop()
{
	if (this->list_->empty())
	{
		throw new std::logic_error("BinaryQueue<K, T>::pop(): Zoznam je prazdny");
	}
	if (this->size() > 1)
	{
		std::swap((*this->list_)[0], (*this->list_)[this->size() - 1]);
	}
	PriorityQueueItem<K, T>* item = this->list_->back();
	this->list_->pop_back();
	this->heapifyDown(0);
	T data = item->data();
	delete item;
	return data;
}

template<typename K, typename T>
inline T& BinaryQueue<K, T>::peek()
{
	if (this->list_->empty())
	{
		throw new std::logic_error("BinaryQueue<K, T>::peek(): Zoznam je prazdny");
	}
	return (*this->list_)[0]->data();
}

template<typename K, typename T>
inline const T BinaryQueue<K, T>::peek() const
{
	if (this->list_->empty())
	{
		throw new std::logic_error("BinaryQueue<K, T>::peek(): Zoznam je prazdny");
	}
	return (*this->list_)[0]->data();
}

template<typename K, typename T>
inline K BinaryQueue<K, T>::peekPriority()
{
	if (this->list_->empty())
	{
		throw new std::logic_error("BinaryQueue<K, T>::peekPriority(): Zoznam je prazdny");
	}
	return (*this->list_)[0]->priority();
}

template<typename K, typename T>
inline int BinaryQueue<K, T>::leftSon(const int index)
{
	return 2 * index + 1;
}

template<typename K, typename T>
inline int BinaryQueue<K, T>::righSon(const int index)
{
	return 2 * index + 2;
}

template<typename K, typename T>
inline int BinaryQueue<K, T>::parent(const int index)
{
	return (index - 1) / 2;
}

template<typename K, typename T>
inline int BinaryQueue<K, T>::greaterSon(const int index)
{
	PriorityQueueItem<K, T>* lChild = this->leftSon(index) < this->size() ? (*this->list_)[this->leftSon(index)] : nullptr;
	PriorityQueueItem<K, T>* rChild = this->righSon(index) < this->size() ? (*this->list_)[this->righSon(index)] : nullptr;
	if (lChild && rChild)
	{
		return lChild->priority() < rChild->priority() ? this->leftSon(index) : this->righSon(index);
	}
	return this->leftSon(index);
}

template<typename K, typename T>
inline void BinaryQueue<K, T>::heapifyUp(const int index)
{
	for (int i = index, parent = this->parent(i); i > 0 && (*this->list_)[parent]->priority() > (*this->list_)[i]->priority(); i = parent, parent = this->parent(i))
	{
		std::swap((*this->list_)[parent], (*this->list_)[i]);
	}
}

template<typename K, typename T>
inline void BinaryQueue<K, T>::heapifyDown(const int index)
{
	for (int i = index, child = this->greaterSon(i); child < this->size() && (*this->list_)[child]->priority() < (*this->list_)[i]->priority(); i = child, child = this->greaterSon(i))
	{
		std::swap((*this->list_)[child], (*this->list_)[i]);
	}
}

