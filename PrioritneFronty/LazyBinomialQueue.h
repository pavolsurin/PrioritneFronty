#pragma once
#include "ExplicitPriorityQueue.h"

/// <summary>
/// Abstraktn� predok pre prioritn� fronty implementovan� lesom bin�rnych stromov
/// </summary>
/// <typeparam name="Priority">D�tov� typ priority</typeparam>
/// <typeparam name="Data">D�tov� typ d�t</typeparam>
template <typename Priority, typename Data>
class LazyBinomialHeap : public ExplicitPriorityQueue<Priority, Data>
{
protected:
	/// <summary>
	/// Pripoj� prvok k atrib�tu root_ a zv�si po�et prvkov
	/// </summary>
	/// <param name="node">Vkladan� prvok</param>
	PriorityQueueItem<Priority, Data>* push(BinaryTreeItem<Priority, Data>* node);
	/// <summary>
	/// Pripoj� prvok k atrib�tu root_
	/// </summary>
	/// <param name="node">Vkladan� prvok</param>
	void add_root_item(BinaryTreeItem<Priority, Data>* node);
	/// <summary>
	/// Zl��i prvky v pravej chrbtici atrib�tu root_ a parametra node
	/// </summary>
	/// <param name="node">Prv� prvok v postupnosti prvkov, ktor� sa maj� zl��i�</param>
	virtual void consolidate_root(BinaryTreeItem<Priority, Data>* node) = 0;
	/// <summary>
	/// Zl��i prvky v pravej chrbtici atrib�tu root_ a parametra node viacprechodovou strat�giou
	/// </summary>
	/// <param name="node">Prv� prvok v postupnosti prvkov, ktor� sa maj� zl��i�</param>
	/// <param name="array_size">Ve�kos� po�a v ktorom sa bude zlu�ova�</param>
	void consolidate_root_using_multipass(BinaryTreeItem<Priority, Data>* node, size_t array_size);
	/// <summary>
	/// Zl��i prvky v pravej chrbtici atrib�tu root_ a parametra node jednoprechodovou strat�giou
	/// </summary>
	/// <param name="node">Prv� prvok v postupnosti prvkov, ktor� sa maj� zl��i�</param>
	/// <param name="array_size">Ve�kos� po�a v ktorom sa bude zlu�ova�</param>
	void consolidate_root_using_onepass(BinaryTreeItem<Priority, Data>* node, size_t array_size);
	/// <summary>
	/// Kon�truktor
	/// </summary>
	LazyBinomialHeap();
public:
	/// <summary>
	/// De�truktor
	/// </summary>
	~LazyBinomialHeap();
	/// <summary>
	/// Vym�e v�etky prvky z prioritn�ho frontu
	/// </summary>
	void clear() override;
	/// <summary>
	/// Vlo�� d�ta do prioritn�ho frontu
	/// </summary>
	/// <param name="identifier">Identifik�tor prvku</param>
	/// <param name="priority">Priorita</param>
	/// <param name="data">Data</param>
	/// <param name="data_item">Vytvoren� prvok</param>
	void push(const int identifier, const Priority& priority, const Data& data, PriorityQueueItem<Priority, Data>*&) = 0;
	/// <summary>
	/// Vyberie z prioritn�ho frontu d�ta s najv��ou prioritou
	/// </summary>
	/// <param name="identifier">Identifik�tor prvku s najv��ou prioritou</param>
	/// <returns>Hodnota d�t</returns>virtual Data pop(int& identifier) = 0;
	Data pop(int& identifier) override;
	/// <summary>
	/// Pripoj� k prioritn�mu frontu prvky z other_heap
	/// </summary>
	/// <param name="other_heap">Prioritn� front, ktor�ho prvky maj� by� pripojen�</param>
	void merge(PriorityQueue<Priority, Data>* other_heap) override;
};

template<typename Priority, typename Data>
inline void LazyBinomialHeap<Priority, Data>::consolidate_root_using_multipass(BinaryTreeItem<Priority, Data>* node, size_t array_size)
{
	std::vector<BinaryTreeItem<Priority, Data>*> node_list(array_size);
	size_t node_degree;

	if (node)
	{
		for (BinaryTreeItem<Priority, Data>* node_ptr = node, *node_next_ptr = node_ptr->right_son(); node_ptr;
			node_ptr = node_next_ptr, node_next_ptr = node_ptr ? node_ptr->right_son() : nullptr)
		{
			node_ptr->cut();
			node_degree = ((DegreeBinaryTreeItem<Priority, Data>*)node_ptr)->degree();

			while (node_list[node_degree])
			{
				node_ptr = node_ptr->merge(node_list[node_degree]);
				node_list[node_degree++] = nullptr;
			}
			node_list[node_degree] = node_ptr;
		}
	}

	if (this->root_)
	{
		int size = this->size_;
		BinaryTreeItem<Priority, Data>* node_ptr = this->root_->right_son();
		this->root_->right_son() = nullptr;

		for (BinaryTreeItem<Priority, Data>* node_next_ptr = node_ptr->right_son(); node_ptr != this->root_;
			node_ptr = node_next_ptr, node_next_ptr = node_ptr->right_son())
		{
			node_ptr->right_son() = nullptr;
			node_degree = ((DegreeBinaryTreeItem<Priority, Data>*)node_ptr)->degree();

			while (node_list[node_degree])
			{
				node_ptr = node_ptr->merge(node_list[node_degree]);
				node_list[node_degree++] = nullptr;
			}
			node_list[node_degree] = node_ptr;
		}

		if (!this->root_->parent())
		{
			node_ptr = this->root_;
			node_degree = ((DegreeBinaryTreeItem<Priority, Data>*)node_ptr)->degree();

			while (node_list[node_degree])
			{
				node_ptr = node_ptr->merge(node_list[node_degree]);
				node_list[node_degree++] = nullptr;
			}
			node_list[node_degree] = node_ptr;
		}

		this->root_ = nullptr;
	}

	for (BinaryTreeItem<Priority, Data>* node : node_list)
	{
		if (node)
		{
			this->add_root_item(node);
		}
	}
}

template<typename Priority, typename Data>
inline void LazyBinomialHeap<Priority, Data>::consolidate_root_using_onepass(BinaryTreeItem<Priority, Data>* node, size_t array_size)
{
	std::vector<BinaryTreeItem<Priority, Data>*> node_list(array_size);
	BinaryTreeItem<Priority, Data>* root = this->root_;
	size_t node_degree;
	this->root_ = nullptr;

	if (node)
	{
		for (BinaryTreeItem<Priority, Data>* node_ptr = node, *node_next_ptr = node_ptr->right_son(); node_ptr;
			node_ptr = node_next_ptr, node_next_ptr = node_ptr ? node_ptr->right_son() : nullptr)
		{
			node_ptr->cut();
			node_degree = ((DegreeBinaryTreeItem<Priority, Data>*)node_ptr)->degree();

			if (node_list[node_degree])
			{
				node_ptr = node_ptr->merge(node_list[node_degree]);
				node_list[node_degree] = nullptr;
				this->add_root_item(node_ptr);
			}
			else
			{
				node_list[node_degree] = node_ptr;
			}
		}
	}

	if (root)
	{
		BinaryTreeItem<Priority, Data>* node_ptr = root->right_son();
		root->right_son() = nullptr;

		for (BinaryTreeItem<Priority, Data>* node_next_ptr = node_ptr->right_son(); node_ptr != root;
			node_ptr = node_next_ptr, node_next_ptr = node_ptr->right_son())
		{
			node_ptr->right_son() = nullptr;

			node_degree = ((DegreeBinaryTreeItem<Priority, Data>*)node_ptr)->degree();

			if (node_list[node_degree])
			{
				node_ptr = node_ptr->merge(node_list[node_degree]);
				node_list[node_degree] = nullptr;
				this->add_root_item(node_ptr);
			}
			else
			{
				node_list[node_degree] = node_ptr;
			}
		}

		if (!root->parent())
		{
			node_ptr = root;
			node_degree = ((DegreeBinaryTreeItem<Priority, Data>*)node_ptr)->degree();

			if (node_list[node_degree])
			{
				node_ptr = node_ptr->merge(node_list[node_degree]);
				node_list[node_degree] = nullptr;
				this->add_root_item(node_ptr);
			}
			else
			{
				node_list[node_degree] = node_ptr;
			}
		}
	}

	for (BinaryTreeItem<Priority, Data>* node : node_list)
	{
		if (node)
		{
			this->add_root_item(node);
		}
	}
}

template<typename Priority, typename Data>
inline LazyBinomialHeap<Priority, Data>::LazyBinomialHeap() :
	ExplicitPriorityQueue<Priority, Data>()
{
}

template<typename Priority, typename Data>
inline LazyBinomialHeap<Priority, Data>::~LazyBinomialHeap()
{
	this->clear();
}

template<typename Priority, typename Data>
inline void LazyBinomialHeap<Priority, Data>::clear()
{
	if (this->root_)
	{
		BinaryTreeItem<Priority, Data>* root = this->root_->right_son();
		this->root_->right_son(nullptr);
		delete root;
	}
	this->root_ = nullptr;
	this->size_ = 0;
}


template<typename Priority, typename Data>
inline PriorityQueueItem<Priority, Data>* LazyBinomialHeap<Priority, Data>::push(BinaryTreeItem<Priority, Data>* node)
{
	this->add_root_item(node);
	this->size_++;
	return node;
}

template<typename Priority, typename Data>
inline void LazyBinomialHeap<Priority, Data>::add_root_item(BinaryTreeItem<Priority, Data>* node)
{
	if (this->root_)
	{
		if (node->right_son())
		{
			std::swap(this->root_->right_son(), node->right_son());
		}
		else
		{
			node->right_son() = this->root_->right_son();
			this->root_->right_son() = node;
		}
		if (*node < *this->root_)
		{
			this->root_ = node;
		}
	}
	else
	{
		this->root_ = node;
		this->root_->right_son() = this->root_;
	}
}

template<typename Priority, typename Data>
inline Data LazyBinomialHeap<Priority, Data>::pop(int& identifier)
{
	if (this->root_)
	{
		BinaryTreeItem<Priority, Data>* root = this->root_;
		this->root_->parent() = this->root_;
		this->consolidate_root(root->left_son());
		root->left_son(nullptr);
		this->size_--;
		Data data = root->data();
		identifier = root->identifier();
		delete root;
		return data;
	}
	throw new std::out_of_range("LazyBinomialHeap<Priority, Data>::pop(): Priority queue is empty!");
}

template<typename Priority, typename Data>
inline void LazyBinomialHeap<Priority, Data>::merge(PriorityQueue<Priority, Data>* other_heap)
{
	if (other_heap)
	{
		LazyBinomialHeap<Priority, Data>* heap = (LazyBinomialHeap<Priority, Data>*)other_heap;
		this->add_root_item(heap->root_);
		this->size_ += heap->size_;
		heap->root_ = nullptr;
		delete other_heap;
	}
}
