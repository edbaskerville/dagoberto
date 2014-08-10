#include "catch.hpp"
#include "dagoberto.hpp"
#include "MatrixMultiplexer.hpp"

using namespace dagoberto;
using namespace std;
using namespace Eigen;

TEST_CASE("Simple matrix multiplex", "[dagoberto]")
{
	Graph g;
	
	g.beginInitialization();
	
	std::vector<std::vector<Value<double>>> mat(2);
	mat[0].reserve(3);
	mat[0].emplace_back(&g, 1);
	mat[0].emplace_back(&g, 2);
	mat[0].emplace_back(&g, 3);
	
	mat[1].reserve(3);
	mat[1].emplace_back(&g, 4);
	mat[1].emplace_back(&g, 5);
	mat[1].emplace_back(&g, 6);
	
	MatrixMultiplexer<double> multiMat(&g, mat);
	
	g.endInitialization(&multiMat);
	
	MatrixXd value = multiMat();
	REQUIRE(value(0,0) == 1);
	REQUIRE(value(0,1) == 2);
	REQUIRE(value(0,2) == 3);
	REQUIRE(value(1,0) == 4);
	REQUIRE(value(1,1) == 5);
	REQUIRE(value(1,2) == 6);
	
	g.beginTransaction();
	mat[0][0] = 50;
	g.endTransaction();
	g.commit();
	value = multiMat();
	REQUIRE(value(0,0) == 50);
	
	g.beginTransaction();
	mat[0][0] = 49;
	g.endTransaction();
	g.rollback();
	value = multiMat();
	REQUIRE(value(0,0) == 50);
}
