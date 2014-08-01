#ifndef __dagoberto__Sum__
#define __dagoberto__Sum__

#include <cassert>
#include <iostream>
#include "Graph.hpp"


namespace dagoberto
{

template<class T>
class Sum : virtual public Node<T>
{
public:
	Sum() : Node<T>()
	{
	}
	
	Sum(Graph * graph, Node<T> * left, Node<T> * right):
		Node<T>(graph, {left, right}), _left(left), _right(right)
	{
	}
	
	virtual T operator()()
	{
		if(NodeBase::isDirty()) {
			_value = (*_left)() + (*_right)();
			NodeBase::setClean();
		}
		return _value;
	}
private:
	T _value;
	Node<T> * _left;
	Node<T> * _right;
};

template<class T>
class VectorSum : virtual public Node<T>
{
public:
	VectorSum() : Node<T>()
	{
	}
	
	VectorSum(Graph * graph, std::vector<Node<T> *> const & terms):
		Node<T>(graph)
	{
		if(terms.size() < 2) {
			throw std::runtime_error("VectorSum requires at least two terms.");
		}
		
		root = makeSum(graph, terms, 0, terms.size());
		graph->addEdge(root, this);
	}
	
	virtual T operator()()
	{
		if(NodeBase::isDirty()) {
			_value = (*root)();
			NodeBase::setClean();
		}
		return _value;
	}
private:
	T _value;
	Sum<T> * root;
	std::vector<std::unique_ptr<Sum<T>>> _sums;
	
	Sum<T> * makeSum(Graph * graph, std::vector<Node<T> *> const & terms, size_t lower, size_t upper)
	{
		size_t size = upper - lower;
		
		size_t nLeft = size / 2;
		assert(nLeft > 0 && nLeft < size);
		size_t nRight = size - nLeft;
		assert(nRight > 0 && nRight < size);
		
		size_t rightLower = lower + nLeft;
		
		Node<T> * leftNode;
		if(nLeft == 1) {
			leftNode = terms[lower];
		}
		else {
			leftNode = makeSum(graph, terms, lower, rightLower);
		}
		
		Node<T> * rightNode;
		if(nRight == 1) {
			rightNode = terms[rightLower];
		}
		else {
			rightNode = makeSum(graph, terms, rightLower, upper);
		}
		
		_sums.emplace_back(new Sum<T>(graph, leftNode, rightNode));
		return _sums.back().get();
	}
};

} // namespace dagoberto

#endif /* defined(__dagoberto__Sum__) */
