#pragma once
#include "LazyBinomialQueue.h"

/// <summary>
/// �rov�ov� p�rovacia halda
/// </summary>
/// <typeparam name="Priority">D�tov� typ priority</typeparam>
/// <typeparam name="Data">D�tov� typ d�t</typeparam>
template <typename Priority, typename Data>
class RankPairingHeap : public LazyBinomialHeap<Priority, Data>
{
private:
	/// <summary>
	/// Nastol� stup�ov� pravidlo od prvku smerom ku kore�u
	/// </summary>
	/// <param name="node"></param>
	void restore_degree_rule(DegreeBinaryTreeItem<Priority, Data>* node);
protected:
	/// <summary>
	/// Zl��i prvky v pravej chrbtici atrib�tu root_ a parametra node viacprechodovou strat�giou
	/// </summary>
	/// <param name="node">Prv� prvok v postupnosti prvkov, ktor� sa maj� zl��i�</param>
	void consolidate_root(BinaryTreeItem<Priority, Data>* node) override;
	/// <summary>
	/// Vystrihne prvok a nasatav� stup�ov� pravidlo od priameho predka
	/// </summary>
	/// <param name="node">Prvok so zv��enou prioritou</param>
	void priority_was_increased(PriorityQueueItem<Priority, Data>* node) override;
	/// <summary>
	/// Vystrihne potomkov prvku s vy��ou prioritou a nastav� stup�ov� pravidlo od priameho predka posledn�ho vystrihnut�ho prvku
	/// </summary>
	/// <param name="node">Prvok so zn�enou prioritou</param>
	void priority_was_decreased(PriorityQueueItem<Priority, Data>* node) override;
public:

	RankPairingHeap();
	~RankPairingHeap();
	void push(const int identifier, const Priority& priority, const Data& data, PriorityQueueItem<Priority, Data>*& data_item) override;

	Data pop(int& identifier) override { return this->LazyBinomialHeap<Priority, Data>::pop(identifier); };
	void change_priority(PriorityQueueItem<Priority, Data>* node, const Priority& priority) override { this->PriorityQueue<Priority, Data>::change_priority(node, priority); };
};

template<typename Priority, typename Data>
inline RankPairingHeap<Priority, Data>::RankPairingHeap() :
	LazyBinomialHeap<Priority, Data>()
{
}

template<typename Priority, typename Data>
inline RankPairingHeap<Priority, Data>::~RankPairingHeap()
{
}

template<typename Priority, typename Data>
inline void RankPairingHeap<Priority, Data>::push(const int identifier, const Priority& priority, const Data& data, PriorityQueueItem<Priority, Data>*& data_item)
{
	data_item = this->LazyBinomialHeap<Priority, Data>::push(new DegreeBinaryTreeItem<Priority, Data>(identifier, priority, data));
}

template<typename Priority, typename Data>
inline void RankPairingHeap<Priority, Data>::restore_degree_rule(DegreeBinaryTreeItem<Priority, Data>* node)
{
	if (node)
	{
		DegreeBinaryTreeItem<Priority, Data>* node_ptr = node;
		int degree, left_son_degree, right_son_degree;
		while (true)
		{
			left_son_degree = node_ptr->left_son() ? ((DegreeBinaryTreeItem<Priority, Data>*)node_ptr->left_son())->degree() : -1;
			right_son_degree = node_ptr->right_son() && node_ptr->right_son()->parent() ? ((DegreeBinaryTreeItem<Priority, Data>*)node_ptr->right_son())->degree() : -1;
			if (node_ptr->parent())
			{
				if (abs(left_son_degree - right_son_degree) > 1)
				{
					degree = std::max(left_son_degree, right_son_degree);
				}
				else
				{
					degree = std::max(left_son_degree, right_son_degree) + 1;
				}
				if (node_ptr->degree() < degree)
				{
					return;
				}
				else
				{
					node_ptr->degree() = degree;
				}
				node_ptr = (DegreeBinaryTreeItem<Priority, Data>*)node_ptr->parent();
			}
			else
			{
				node_ptr->degree() = left_son_degree + 1;
				return;
			}
		}
	}
}

template<typename Priority, typename Data>
inline void RankPairingHeap<Priority, Data>::consolidate_root(BinaryTreeItem<Priority, Data>* node)
{
	this->consolidate_root_using_multipass(node, (int)(log(this->size_) * 2.1) + 2);
}

template<typename Priority, typename Data>
inline void RankPairingHeap<Priority, Data>::priority_was_increased(PriorityQueueItem<Priority, Data>* node)
{
	DegreeBinaryTreeItem<Priority, Data>* casted_node = (DegreeBinaryTreeItem<Priority, Data>*)node;
	casted_node->degree() = casted_node->left_son() ? ((DegreeBinaryTreeItem<Priority, Data>*)casted_node->left_son())->degree() + 1 : 0;
	if (casted_node->parent())
	{
		DegreeBinaryTreeItem<Priority, Data>* parent = (DegreeBinaryTreeItem<Priority, Data>*)casted_node->parent();
		this->add_root_item(casted_node->cut());
		this->restore_degree_rule(parent);
	}
	else if (*casted_node < *this->root_)
	{
		this->root_ = casted_node;
	}
}

template<typename Priority, typename Data>
inline void RankPairingHeap<Priority, Data>::priority_was_decreased(PriorityQueueItem<Priority, Data>* node)
{
	BinaryTreeItem<Priority, Data>* last_change = nullptr;
	if (this->root_ == node)
	{
		BinaryTreeItem<Priority, Data>* new_root = this->root_;
		for (BinaryTreeItem<Priority, Data>* node_ptr = this->root_->right_son(); node_ptr != this->root_; node_ptr = node_ptr->right_son())
		{
			if (*node_ptr < *new_root)
			{
				new_root = node_ptr;
			}
		}
		this->root_ = new_root;
	}
	for (BinaryTreeItem<Priority, Data>* node_ptr = ((BinaryTreeItem<Priority, Data>*)node)->left_son(), *node_next_ptr = node_ptr ? node_ptr->right_son() : nullptr; node_ptr;
		node_ptr = node_next_ptr, node_next_ptr = node_ptr ? node_ptr->right_son() : nullptr)
	{
		if (*node_ptr < *node)
		{
			last_change = node_ptr->parent();
			((DegreeBinaryTreeItem<Priority, Data>*)node_ptr->cut())->degree() = node_ptr->left_son() ? ((DegreeBinaryTreeItem<Priority, Data>*)node_ptr->left_son())->degree() + 1 : 0;
			this->add_root_item(node_ptr);
		}
	}
	this->restore_degree_rule((DegreeBinaryTreeItem<Priority, Data>*)last_change);
}
