all: build ewcc pls sbts

build:
	mkdir -p classic_solvers/build

ewcc: build
	g++ -std=c++17 -O3 -o classic_solvers/build/ewcc classic_solvers/ewcc/test_ewcc.cpp

pls: build
	g++ -std=c++17 -O3 -o classic_solvers/build/pls classic_solvers/pls/test_pls.cpp

sbts: build
	g++ -std=c++17 -O3 -o classic_solvers/build/sbts classic_solvers/sbts/test_sbts.cpp

clean:
	rm -f -r classic_solvers/build