.Phony: zip clean all

NAME=exc06_csaw1585
EXCLUDE_FILES = "**.vscode/*" "**.idea/*" "**__MACOSX/*" "**.DS_Store/*" "**.dSYM/*" "README.md" "*.sh" "*.out"

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