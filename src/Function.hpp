#ifndef __dagoberto__Function__
#define __dagoberto__Function__

#include <cassert>
#include "Graph.hpp"

namespace dagoberto
{

template<class T>
class Function : virtual public Node<T>
{
public:
	Function():
		Node<T>()
	{
	}
	
	Function(Graph * graph, std::vector<NodeBase *> const & dependencies, std::function<T()> func):
		Node<T>(graph, dependencies),
		_func(func)
	{
	}
	
	virtual T operator()()
	{
		if(NodeBase::isDirty()) {
			_value = _func();
			NodeBase::setClean();
		}
		return _value;
	}
private:
	std::function<T()> _func;
	T _value;
};

} // namespace dagoberto

#endif /* defined(__dagoberto__Function__) */
