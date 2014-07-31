#ifndef __dagoberto__Graph__
#define __dagoberto__Graph__

#include <cassert>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace dagoberto {

class Graph;
class NodeBase;

class Graph {
public:
	Graph();
	void addEdge(NodeBase * from, NodeBase * to);
	std::unordered_set<NodeBase *> & edges(NodeBase * edge);
private:
	std::unordered_map<NodeBase *, std::unordered_set<NodeBase *>> _edges;
};

class NodeBase {
public:
	NodeBase(Graph * graph);
	NodeBase(Graph * graph, std::vector<NodeBase *> const & dependencies);
	bool isDirty();
protected:
	void setDirty();
	void setClean();
private:
	Graph * _graph;
	bool _dirty;
};

template<class T>
class Node : public NodeBase
{
public:
	Node(Graph * graph):
		NodeBase(graph)
	{
	}
	
	Node(Graph * graph, std::vector<NodeBase *> const & dependencies):
		NodeBase(graph, dependencies)
	{
	}
	
	virtual T operator()() = 0;
};

} // namespace dagoberto

#endif /* defined(__dagoberto__Graph__) */
