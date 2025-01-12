clean:
	rm *.out
main:
	clang++ -std=c++11 main.cpp parser.cpp toanf.cpp toselect.cpp

