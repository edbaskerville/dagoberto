#include "catch.hpp"
#include "dagoberto.hpp"

using namespace dagoberto;
using namespace std;

TEST_CASE("example", "[dagoberto]")
{
	double const tol = 1e-3;
	
	Graph g;
	
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
	
	double Hval = H();
	REQUIRE(Hval > 151.5 - tol);
	REQUIRE(Hval < 151.5 + tol);
	
	N = 5;
	
	Hval = H();
	REQUIRE(Hval > 5191.5 - tol);
	REQUIRE(Hval < 5191.5 + tol);
	
	A = 1.0;
	
	Hval = H();
	REQUIRE(Hval > 1095.5 - tol);
	REQUIRE(Hval < 1095.5 + tol);
	
	B = 2.0;
	
	Hval = H();
	REQUIRE(Hval > 103.5 - tol);
	REQUIRE(Hval < 103.5 + tol);
	
	D = 12345.0;
	
	Hval = H();
	REQUIRE(Hval > 12445 - tol);
	REQUIRE(Hval < 12445 + tol);
	
	F = 23;
	
	Hval = H();
	REQUIRE(Hval > 12435 - tol);
	REQUIRE(Hval < 12435 + tol);
	
	G = 25;
	
	Hval = H();
	REQUIRE(Hval > 12425 - tol);
	REQUIRE(Hval < 12425 + tol);
}
