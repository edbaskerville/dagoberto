#ifndef __dagoberto__Value__
#define __dagoberto__Value__

#include <cassert>
#include "Graph.hpp"

namespace dagoberto
{

template<class T>
class Value : public LeafNode<T> {
public:
	Value() : LeafNode<T>()
	{
	}
	
	Value(Graph * graph, T const & value) :
		LeafNode<T>(graph, value)
	{
	}
	
	virtual T const & evaluate()
	{
		return Node<T>::value();
	}
	
	virtual void operator=(T value)
	{
		Node<T>::setValue(value);
	}
};

} // namespace dagoberto

#endif /* defined(__dagoberto__Value__) */
