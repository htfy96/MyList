.PHONY: prepare clean
gcc98: mylist.cpp
	g++ $^ -o mylist

clang98: mylist.cpp
	clang++ $^ -o mylist

clang11: mylist.cpp
	clang++ --std=c++11 $^ -o mylist

clean:
	rm mylist

prepare:
	ctags mylist.cpp --extra=+p; gtags; htags --suggest
