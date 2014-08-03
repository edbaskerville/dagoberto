#include "catch.hpp"
#include "dagoberto.hpp"

using namespace dagoberto;
using namespace std;

TEST_CASE("example", "[dagoberto]")
{
	double const tol = 1e-3;
	
	Graph g;
	
	g.beginInitialization();
	
	Value<int64_t> N(&g, 2);
	Value<double> A(&g, 5.0);
	Value<double> B(&g, 4.0);
	
	Function<double> C(&g, {&N, &A, &B}, [&N, &A, &B]() -> double {
		double value = 1.0;
		double Nval = N();
		double Bval = B();
		for(int64_t i = 0; i < Nval; i++) {
			value *= Bval;
		}
		return A() * value;
	});
	
	Value<double> D(&g, 3.5);
	
	Sum<double> E(&g, &C, &D);
	
	Value<double> F(&g, 33);
	Value<double> G(&g, 35);
	VectorSum<double> H(&g, {&E, &F, &G});
	
	g.endInitialization(&H);
	
	REQUIRE(H() > 151.5 - tol);
	REQUIRE(H() < 151.5 + tol);
	
	g.beginTransaction();
	N = 5;
	g.endTransaction();
	g.commit();
	
	REQUIRE(H() > 5191.5 - tol);
	REQUIRE(H() < 5191.5 + tol);
	
	REQUIRE(H() > 5191.5 - tol);
	REQUIRE(H() < 5191.5 + tol);
	
	
	g.beginTransaction();
	A = 1.0;
	g.endTransaction();
	g.commit();
	
	REQUIRE(H() > 1095.5 - tol);
	REQUIRE(H() < 1095.5 + tol);
	
	g.beginTransaction();
	B = 2.0;
	g.endTransaction();
	g.commit();
	
	REQUIRE(H() > 103.5 - tol);
	REQUIRE(H() < 103.5 + tol);
	
	g.beginTransaction();
	D = 12345.0;
	g.endTransaction();
	g.commit();
	
	REQUIRE(H() > 12445 - tol);
	REQUIRE(H() < 12445 + tol);
	
	g.beginTransaction();
	F = 23;
	g.endTransaction();
	g.commit();
	
	REQUIRE(H() > 12435 - tol);
	REQUIRE(H() < 12435 + tol);
	
	g.beginTransaction();
	G = 25;
	g.endTransaction();
	g.commit();
	
	REQUIRE(H() > 12425 - tol);
	REQUIRE(H() < 12425 + tol);
	
	g.beginTransaction();
	G = 45;
	D = 10;
	g.endTransaction();
	REQUIRE(H() > 110 - tol);
	REQUIRE(H() < 110 + tol);
	g.rollback();
	
	REQUIRE(H() > 12425 - tol);
	REQUIRE(H() < 12425 + tol);
	
	g.beginTransaction();
	G = 45;
	D = 10;
	g.endTransaction();
	g.commit();
	REQUIRE(H() > 110 - tol);
	REQUIRE(H() < 110 + tol);
}
