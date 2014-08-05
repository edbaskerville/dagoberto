#include "Graph.hpp"

using namespace std;

namespace dagoberto {

Graph::Graph() :
	_state(GraphState::UNINITIALIZED)
{
}

void Graph::beginInitialization()
{
	if(_state != GraphState::UNINITIALIZED) {
		throw runtime_error("beginIntialization can only happen once.");
	}
	_state = GraphState::INITIALIZING;
}

void Graph::addNode(NodeBase * node)
{
	_nodes.insert(node);
	if(!node->isLeaf()) {
		_dirtyNodes.insert(node);
	}
}

void Graph::addEdge(NodeBase * from, NodeBase * to)
{
	if(_state != GraphState::INITIALIZING) {
		throw runtime_error("addEdge can only happen during initialization.");
	}
	if(_nodes.find(from) == _nodes.end()) {
		throw runtime_error("from node not present in graph");
	}
	if(_nodes.find(to) == _nodes.end()) {
		throw runtime_error("to node not present in graph");
	}
	if(from == to) {
		throw runtime_error("node cannot depend on itself");
	}
	_edges[from].insert(to);
}

void Graph::endInitialization(NodeBase * root)
{
	if(_state != GraphState::INITIALIZING) {
		throw runtime_error("endInitialization can only happen when in initialization");
	}
	if(_nodes.find(root) == _nodes.end()) {
		throw runtime_error("root not present in graph");
	}
	for(auto & pair : _edges) {
		NodeBase * from = pair.first;
		if(from == root) {
			throw runtime_error("No node can depend on the value of the root");
		}
	}
	_root = root;
	_state = GraphState::RECALCULATING;
	_root->recalculate();
	_visitedNodes.clear();
	_state = GraphState::UNCOMMITTED;
	commit();
}

void Graph::beginTransaction()
{
	if(_state != GraphState::READY) {
		throw runtime_error("beginTransaction can only happen when ready");
	}
	assert(_dirtyNodes.size() == 0);
	assert(_visitedNodes.size() == 0);
	_state = GraphState::IN_TRANSACTION;
}

void Graph::endTransaction()
{
	if(_state != GraphState::IN_TRANSACTION) {
		throw runtime_error("endTransaction can only happen during transaction");
	}
	_state = GraphState::RECALCULATING;
	_root->recalculate();
	_visitedNodes.clear();
	_state = GraphState::UNCOMMITTED;
}

void Graph::commit()
{
	if(_state != GraphState::UNCOMMITTED) {
		throw runtime_error("commit can only happen after transaction");
	}
	for(NodeBase * nodePtr : _dirtyNodes) {
		nodePtr->commit();
	}
	_dirtyNodes.clear();
	_state = GraphState::READY;
}

void Graph::rollback()
{
	if(_state != GraphState::UNCOMMITTED) {
		throw runtime_error("commit can only happen after transaction");
	}
	for(NodeBase * nodePtr : _dirtyNodes) {
		nodePtr->rollback();
	}
	_dirtyNodes.clear();
	_state = GraphState::READY;
}

void Graph::recalculateAll()
{
	if(_state != GraphState::READY) {
		throw runtime_error("full recalculation can only happen when ready");
	}
	beginTransaction();
	for(NodeBase * nodePtr : _nodes) {
		setDirty(nodePtr);
	}
	endTransaction();
	commit();
}

bool Graph::isDirty(NodeBase * node)
{
	return _dirtyNodes.find(node) != _dirtyNodes.end();
}

void Graph::setDirty(NodeBase * nodePtr)
{
	if(_dirtyNodes.find(nodePtr) == _dirtyNodes.end()) {
		_dirtyNodes.insert(nodePtr);
		for(NodeBase * depPtr : _edges[nodePtr]) {
			setDirty(depPtr);
		}
	}
}

bool Graph::isVisited(NodeBase * nodePtr)
{
	assert(_state == GraphState::RECALCULATING);
	return _visitedNodes.find(nodePtr) != _visitedNodes.end();
}

NodeBase::NodeBase():
	_graph(nullptr), _isLeaf(false)
{
}

NodeBase::NodeBase(bool isLeaf, Graph * graph) :
	_isLeaf(isLeaf), _graph(graph)
{
	if(_graph == nullptr) {
		throw runtime_error("Graph is null");
	}
	
	graph->addNode(this);
}
	
NodeBase::NodeBase(Graph * graph, std::vector<NodeBase *> const & dependencies) :
	_graph(graph), _isLeaf(false)
{
	if(_graph == nullptr) {
		throw runtime_error("Graph is null");
	}
	
	graph->addNode(this);
	for(auto dep : dependencies) {
		graph->addEdge(dep, this);
	}
}

GraphState NodeBase::graphState()
{
	return _graph->_state;
}

bool NodeBase::isLeaf()
{
	return _isLeaf;
}

bool NodeBase::isDirty()
{
	if(_graph == nullptr) {
		throw runtime_error("Graph is null");
	}
	return _graph->isDirty(this);
}

void NodeBase::setDirty()
{
	if(_graph == nullptr) {
		throw runtime_error("Graph is null");
	}
	_graph->setDirty(this);
}

bool NodeBase::isVisited()
{
	if(_graph == nullptr) {
		throw runtime_error("Graph is null");
	}
	return _graph->isVisited(this);
}

} // namespace dagoberto
