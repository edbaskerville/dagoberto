#include "Graph.hpp"

using namespace std;

namespace dagoberto {

Graph::Graph()
{
}

void Graph::addEdge(NodeBase * from, NodeBase * to)
{
	_edges[from].insert(to);
}

std::unordered_set<NodeBase *> & Graph::edges(NodeBase * node)
{
	return _edges[node];
}

NodeBase::NodeBase():
	_graph(NULL),
	_dirty(false)
{
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
	if(!_dirty) {
		_dirty = true;
		for(auto nodePtr : _graph->edges(this)) {
			nodePtr->setDirty();
		}
	}
}

void NodeBase::setClean()
{
	_dirty = false;
}

} // namespace dagoberto
