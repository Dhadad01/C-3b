#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60
#define TEN 10
#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder in case there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case there is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;
static void print_func_int(void *x){
  {
    int *xi = (int*)(x) ;
    printf ("[%d]", *xi);
    if (*xi != BOARD_SIZE)
    {
      int first, second;
      for(int i=0;i<NUM_OF_TRANSITIONS;i++)
      {
        first = transitions[i][0];
        second = transitions[i][1];
        if ((first == *xi) && (first > second)) // its a ladder
        {
          printf("-snake to %d -> ",second);
          return;
        }
        else if ((first == *xi) && (first < second)) // its a ladder
        {
          printf("-ladder to %d -> ",second);
          return;
        }
      }
    }
    if (*xi != BOARD_SIZE)
    {
      printf(" -> "); // didnt finished - -> to next
    }
  }
}
static int comp_func_int(void *x,void*y){
  int xi = *((int*)(x));
  int yi = *((int*)(y));
  return (xi-yi);
}
static void free_data_int(void *x){
  int *xi = (int*)(x);
  free (xi);
}
static void* copy_int (void *x)
{
  int *xi = (int*)(x);
  int *new_num = malloc(sizeof(xi));
  if(!new_num) // allocation failed
  {
    return NULL;
  }
  *new_num=*xi;
  return new_num;
}
static bool is_last_int (void *x){
  int xi = *((int*)(x));
  if (xi==BOARD_SIZE){
    return true;
  }
  return false;
}
/** Error handler **/
static int handle_error(char *error_msg, MarkovChain **database)
{
    printf("%s", error_msg);
    if (database != NULL)
    {
        free_markov_chain(database);
    }
    return EXIT_FAILURE;
}


static int create_board(Cell *cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++) {
                free(cells[j]);
            }
            handle_error(ALLOCATION_ERROR_MASSAGE,NULL);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        }
        else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database(MarkovChain *markov_chain)
{
    Cell* cells[BOARD_SIZE];
    if(create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        add_to_database(markov_chain, cells[i]);
    }

    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        from_node = get_node_from_database(markov_chain,cells[i])->data;

        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            index_to = MAX(cells[i]->snake_to,cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain, cells[index_to])
                    ->data;
            add_node_to_counter_list(from_node, to_node, markov_chain);
        }
        else
        {
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ((Cell*) (from_node->data))->number + j - 1;
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }
                to_node = get_node_from_database(markov_chain, cells[index_to])
                        ->data;
                add_node_to_counter_list(from_node, to_node, markov_chain);
            }
        }
    }
    // free temp arr
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EXIT_FAILURE;
  }
  unsigned int seed = (unsigned int) strtol
      (argv[1], NULL, TEN);
  int num_of_sentences = (int) strtol
      (argv[2],NULL, TEN);

  srand (seed);
  // init the markov chain
  MarkovChain *markov_chain =(MarkovChain *)malloc(sizeof (MarkovChain));
  markov_chain->database = (LinkedList *) malloc (sizeof (LinkedList));
  markov_chain->database->first = NULL;
  markov_chain->database->last = NULL;
  markov_chain->database->size = 0;
  markov_chain->print_func = print_func_int;
  markov_chain->free_data = free_data_int;
  markov_chain->copy_func = copy_int;
  markov_chain->comp_func = comp_func_int;
  markov_chain->is_last = is_last_int;
  int fill = fill_database (markov_chain);
  if (fill)
  {
    free_markov_chain(&markov_chain);
    fprintf (stdout, ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  for (int i = 0; i < num_of_sentences; i++)
  {
    printf ("Random Walk %d: ", i + 1);
    MarkovNode *first = markov_chain->database->first->data;
    generate_random_sequence (markov_chain, first
        , MAX_GENERATION_LENGTH);
    printf ("\n");
  }



  free_markov_chain (&markov_chain);
  return EXIT_SUCCESS;
}
