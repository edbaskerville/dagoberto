#include "Graph.hpp"
#include <iostream>

using namespace std;

namespace dagoberto {

Graph::Graph()
{
}

void Graph::addEdge(NodeBase * from, NodeBase * to)
{
	cerr << "adding edge from " << from << " to " << to << endl;
	_edges[from].insert(to);
}

std::unordered_set<NodeBase *> & Graph::edges(NodeBase * node)
{
	return _edges[node];
}

NodeBase::NodeBase(Graph * graph):
	_graph(graph),
	_dirty(true)
{
}
	
NodeBase::NodeBase(Graph * graph, std::vector<NodeBase *> const & dependencies) :
	_graph(graph),
	_dirty(true)
{
	for(auto dep : dependencies) {
		graph->addEdge(dep, this);
	}
}

bool NodeBase::isDirty()
{
	return _dirty;
}

void NodeBase::setDirty()
{
	cerr << "setting dirty " << this << endl;
	if(!_dirty) {
		_dirty = true;
		for(auto nodePtr : _graph->edges(this)) {
			cerr << "making parent " << nodePtr << " dirty" << endl;
			nodePtr->setDirty();
		}
	}
}

void NodeBase::setClean()
{
	cerr << "setting " << this << " clean" << endl;
	_dirty = false;
}

} // namespace dagoberto
