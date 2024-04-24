.Phony: zip clean all

NAME=exc05_csaw1585
EXCLUDE_FILES := README.md

all:
	cd task1 && $(MAKE)
	cd task2 && $(MAKE)
	cd task3 && $(MAKE)

zip:
	$(MAKE) clean
	zip -r $(NAME).zip ./ -x $(EXCLUDE_FILES)

clean:
	$(RM) $(NAME).zip	
	cd task1 && $(MAKE) clean
	cd task2 && $(MAKE) clean
	cd task3 && $(MAKE) clean