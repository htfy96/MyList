.PHONY: prepare
prepare:
	ctags mylist.cpp --extra=+p; gtags; htags --suggest
