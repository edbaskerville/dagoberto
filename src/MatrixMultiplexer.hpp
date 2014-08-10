#ifndef __dagoberto__MatrixMultiplexer__
#define __dagoberto__MatrixMultiplexer__

#include <Eigen/Dense>
#include <cassert>
#include <iostream>
#include "Graph.hpp"
#include <memory>

namespace dagoberto
{

template<typename T>
class MatrixMultiplexer : public InnerNode<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>
{
public:
	MatrixMultiplexer() : InnerNode<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>()
	{
	}
	
	template<typename TVecVec>
	MatrixMultiplexer(
		Graph * graph, TVecVec & nodes
	):
		InnerNode<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>(
			graph, _makeDependencies(nodes)
		),
		_nodes(_makePtrMatrix(nodes)),
		_rows(_nodes.size()),
		_cols(_nodes[0].size())
	{
	}
	
	virtual Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> calculate()
	{
		Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> mat(_rows, _cols);
		for(int64_t i = 0; i < _rows; i++) {
			for(int64_t j = 0; j < _cols; j++) {
				mat(i,j) = (_nodes[i][j])->evaluate();
			}
		}
		
		return mat;
	}
private:
	template<typename TVecVec>
	std::vector<NodeBase *> _makeDependencies(TVecVec & nodes)
	{
		std::vector<NodeBase *> deps;
		for(auto & row : nodes) {
			for(auto & node : row) {
				deps.push_back((NodeBase *)&node);
			}
		}
		assert(deps[0] != nullptr);
		return deps;
	}
	
	template<typename TVecVec>
	std::vector<std::vector<Node<T> *>> _makePtrMatrix(TVecVec & nodes)
	{
		int64_t rows = nodes.size();
		int64_t cols = -1;
		
		std::vector<std::vector<Node<T> *>> ptrMat(rows);
		for(int64_t i = 0; i < rows; i++) {
			if(cols == -1) {
				cols = nodes[i].size();
			}
			else {
				if(nodes[i].size() != cols) {
					throw std::runtime_error("Row lengths don't match");
				}
			}
			ptrMat[i].reserve(cols);
			for(int64_t j = 0; j < cols; j++) {
				ptrMat[i].push_back(&(nodes[i][j]));
			}
		}
		fprintf(stderr, "r, c = %lld, %lld\n", rows, cols);
		
		return ptrMat;
	}
	
	std::vector<std::vector<Node<T> *>> _nodes;
	int64_t _rows;
	int64_t _cols;
};

} // namespace dagoberto

#endif /* defined(__dagoberto__Sum__) */
