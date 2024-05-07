build:
	g++ src.cpp
	-I<PATH_TO_INCLUDE>
	-L<PATH_TO_LIB>
	-lsfml-graphics
	-lsfml-window
	-lsfml-system
	--std=c++11
	-o sfml-app