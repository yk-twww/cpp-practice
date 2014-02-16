
PROGS = train_bigram decode_bigram

all: $(PROGS)

train_bigram: train_bigram.o util.o
	g++ train_bigram.o util.o -o train_bigram

train_bigram.o: train_bigram.cpp
	g++ -O2 -c train_bigram.cpp


decode_bigram: decode_bigram.o util.o
	g++ decode_bigram.o util.o -o decode_bigram

decode_bigram.o: decode_bigram.cpp
	g++ -O2 -c decode_bigram.cpp


util.o: util.cpp
	g++ -c util.cpp


.PHONY: clean
clean:
	rm -f *.o decode_bigram train_bigram
