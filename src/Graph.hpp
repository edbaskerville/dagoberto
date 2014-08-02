#ifndef __dagoberto__Graph__
#define __dagoberto__Graph__

#include <cassert>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

namespace dagoberto {

class Graph;
class NodeBase;

enum class GraphState
{
	UNINITIALIZED,
	INITIALIZING,
	READY,
	IN_TRANSACTION,
	RECALCULATING,
	UNCOMMITTED
};

class Graph {
friend class NodeBase;
public:
	Graph();
	
	void beginInitialization();
	void addNode(NodeBase * nodePtr);
	void addEdge(NodeBase * from, NodeBase * to);
	void endInitialization(NodeBase * root);
	
	void beginTransaction();
	void endTransaction();
	void commit();
	void rollback();
private:
	GraphState _state;
	
	std::unordered_set<NodeBase *> _nodes;
	std::unordered_map<NodeBase *, std::unordered_set<NodeBase *>> _edges;
	
	NodeBase * _root;
	std::unordered_set<NodeBase *> _dirtyNodes;
	std::unordered_set<NodeBase *> _visitedNodes;
	
	void setDirty(NodeBase * nodePtr);
	bool isDirty(NodeBase * nodePtr);
	bool isVisited(NodeBase * nodePtr);
};

class NodeBase {
friend class Graph;
public:
	NodeBase();
	NodeBase(bool isLeaf, Graph * graph);
	NodeBase(Graph * graph, std::vector<NodeBase *> const & dependencies);
	
	virtual void recalculate() = 0;
protected:
	GraphState graphState();
	bool isLeaf();
	bool isDirty();
	void setDirty();
	bool isVisited();
	
	virtual void rollback() = 0;
	virtual void commit() = 0;
private:
	Graph * _graph;
	bool _isLeaf;
};

template<class T>
class Node : public NodeBase
{
public:
	Node() : NodeBase()
	{
	}
	
	Node(bool isLeaf, Graph * graph):
		NodeBase(isLeaf, graph)
	{
	}
	
	Node(bool isLeaf, Graph * graph, T const & value):
		NodeBase(isLeaf, graph), _value(value)
	{
	}
	
	Node(Graph * graph, std::vector<NodeBase *> const & dependencies):
		NodeBase(graph, dependencies)
	{
	}
	
	virtual void recalculate()
	{
		evaluate();
	}
	
	virtual T const & operator()()
	{
		return evaluate();
	}
	
	virtual T const & evaluate() = 0;
	
	virtual void setValue(T const & newValue)
	{
		if(_value != newValue) {
			_newValue = newValue;
			setDirty();
		}
	}
protected:
	virtual T const & value()
	{
		switch(graphState()) {
			case GraphState::RECALCULATING:
			case GraphState::UNCOMMITTED:
				return isDirty() ? _newValue : _value;
			case GraphState::READY:
				return _value;
			default:
				throw std::runtime_error("Error: attempt to get value in wrong state.");
		}
	}
private:
	T _value;
	T _newValue;
	
	virtual void rollback()
	{
		_newValue = T();
	}
	
	virtual void commit()
	{
		_value = _newValue;
		_newValue = T();
	}
};

template<class T>
class InnerNode : public Node<T>
{
public:
	InnerNode() : Node<T>()
	{
		assert(false);
	}
	
	InnerNode(Graph * graph):
		Node<T>(false, graph)
	{
	}
	
	InnerNode(Graph * graph, std::vector<NodeBase *> const & dependencies):
		Node<T>(graph, dependencies)
	{
	}
	
	virtual T const & evaluate()
	{
		switch(NodeBase::graphState()) {
			case GraphState::RECALCULATING:
				if(NodeBase::isDirty() && !NodeBase::isVisited()) {
					Node<T>::setValue(calculate());
				}
				break;
			case GraphState::UNCOMMITTED:
			case GraphState::READY:
				break;
			default:
				throw std::runtime_error("Cannot evaluate in this state");
		}
		return Node<T>::value();
	}
	
	virtual T calculate() = 0;
	
	virtual void setValue(T const & newValue)
	{
		switch(Node<T>::graphState()) {
			case GraphState::RECALCULATING:
				Node<T>::setValue(newValue);
				break;
			default:
				throw std::runtime_error("Error: attempt to set value in wrong state.");
		}
	}
};

template<class T>
class LeafNode : public Node<T>
{
public:
	LeafNode() : Node<T>()
	{
	}
	
	LeafNode(Graph * graph) :
		Node<T>(graph)
	{
	}
	
	LeafNode(Graph * graph, T const & value):
		Node<T>(true, graph, value)
	{
	}
	
	virtual void setValue(T const & newValue)
	{
		switch(Node<T>::graphState()) {
			case GraphState::IN_TRANSACTION:
				Node<T>::setValue(newValue);
				break;
			default:
				throw std::runtime_error("Error: attempt to set value in wrong state.");
		}
	}
};
	
} // namespace dagoberto

#endif /* defined(__dagoberto__Graph__) */
