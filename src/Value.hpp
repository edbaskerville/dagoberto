#ifndef __dagoberto__Value__
#define __dagoberto__Value__

#include <cassert>
#include "Graph.hpp"

namespace dagoberto
{

template<class T>
class Value : virtual public Node<T> {
public:
	Value() : Node<T>()
	{
	}
	
	Value(Graph * graph, T const & value) :
		Node<T>(graph), _value(value)
	{
	}
	
	virtual T operator()()
	{
		NodeBase::setClean();
		return _value;
	}
	
	void operator=(T value)
	{
		if(value != value) {
			_value = value;
			NodeBase::setDirty();
		}
	}
private:
	T _value;
};

} // namespace dagoberto

#endif /* defined(__dagoberto__Value__) */
