#ifndef __dagoberto__Function__
#define __dagoberto__Function__

#include <cassert>
#include "Graph.hpp"

namespace dagoberto
{

template<class T>
class Function : public InnerNode<T>
{
public:
	Function():
		InnerNode<T>()
	{
	}
	
	Function(Graph * graph, std::vector<NodeBase *> const & dependencies, std::function<T()> func):
		InnerNode<T>(graph, dependencies),
		_func(func)
	{
	}
	
	virtual T calculate()
	{
		return _func();
	}
private:
	std::function<T()> _func;
};

} // namespace dagoberto

#endif /* defined(__dagoberto__Function__) */
