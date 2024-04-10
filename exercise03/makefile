.Phony: zip clean all

NAME=exc03_csaw1585

all:
	cd task2 && $(MAKE)
	cd task3 && $(MAKE)

zip:
	$(MAKE) clean
	zip -r $(NAME).zip ./

clean:
	$(RM) exc03_csaw1585.zip	
	cd task2 && $(MAKE) clean
	cd task3 && $(MAKE) clean