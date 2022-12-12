linked_list.o: linked_list.c linked_list.h
	gcc -Wall -Wextra -Wvla -std=c99 -g -c linked_list.c

markov_chain.o: markov_chain.c markov_chain.h
	gcc -Wall -Wextra -Wvla -std=c99 -g -c markov_chain.c

tweets_generator.o: markov_chain.o tweets_generator.c
	gcc -Wall -Wextra -Wvla -std=c99 -g -c tweets_generator.c markov_chain.o

tweets: tweets_generator.o linked_list.o markov_chain.o
	gcc -Wall -Wextra -Wvla -std=c99  tweets_generator.o linked_list.o markov_chain.o -o tweets_generator

snakes_and_ladders.o: markov_chain.o snakes_and_ladders.c
	gcc -Wall -Wextra -Wvla -std=c99 -g -c snakes_and_ladders.c markov_chain.o

snake: snakes_and_ladders.o markov_chain.o linked_list.o
	gcc -Wall -Wextra -Wvla -std=c99  snakes_and_ladders.o markov_chain.o linked_list.o -o snakes_and_ladders





