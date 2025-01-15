clean:
	rm *.out && cd tests && rm *.out
main:
	clang++ -std=c++11 main.cpp parser.cpp toanf.cpp toselect.cpp
test:
	cd tests && clang++ -std=c++23 compiler_tests.cpp ../parser.cpp ../toanf.cpp ../toselect.cpp && ./a.out


