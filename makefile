linked_list.o: linked_list.c linked_list.h
	gcc -c linked_list.c

markov_chain.o: markov_chain.c markov_chain.h
	gcc -c markov_chain.c

tweets_generator.o: markov_chain.o tweets_generator.c
	gcc -c tweets_generator.c markov_chain.o

tweets: tweets_generator.o linked_list.o markov_chain.o
	gcc -c tweets_generator.o linked_list.o markov_chain.o -o tweets_generator

snakes_and_ladders.o: markov_chain.o snakes_and_ladders.c
	gcc -c snakes_and_ladders.c markov_chain.o

snake: snakes_and_ladders.o markov_chain.o linked_list.o
	gcc -c snakes_and_ladders.o markov_chain.o linked_list.o -o snakes_and_ladders





