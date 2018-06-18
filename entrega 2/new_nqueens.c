#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>

MPI_Status estado;
uint64_t TOTAL_SOLUCIONES;
int buffer;
int num_workers;
int id;
int n;

// above this board size overflows may occur
#define MAXN 29

// get the current wall clock time in seconds
double get_time() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return tp.tv_sec + tp.tv_usec / 1000000.0;
}

uint64_t master(uint_fast8_t n) {

  int workersDisponibles=1;

  // counter for the number of solutions PARTIALLY
  uint_fast64_t num = 0;
  uint_fast16_t start_cnt = 0;
  uint_fast32_t start_queens[(MAXN - 2)*(MAXN - 1)][2];
  for (uint_fast8_t q0 = 0; q0 < n - 2; q0++) {
    for (uint_fast8_t q1 = q0 + 2; q1 < n; q1++) {
      start_queens[start_cnt][0] = 1 << q0;
      start_queens[start_cnt][1] = 1 << q1;
      start_cnt++;
    }
  }
  printf("%d!\n", start_cnt);

// La bolsa de tareas generará tareas de tamaño N² para reducir la cantidad de mensajes
  for (uint_fast16_t cnt = 0; cnt < start_cnt; cnt++) {
    int flag=0;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,&flag,&estado);
    // Recibí un SMS
    if(flag){
      MPI_Recv(&buffer,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&estado);
      workersDisponibles++;
      if(buffer){
        TOTAL_SOLUCIONES+=buffer;
      }
      MPI_Send(&cnt,1,MPI_INT,estado.MPI_SOURCE,0,MPI_COMM_WORLD);
      workersDisponibles--;
    } else {
      // Tengo que laburar :S
      int iterar = cnt;
        uint_fast32_t cols[MAXN], posibs[MAXN]; // Our backtracking 'stack'
        uint_fast32_t diagl[MAXN], diagr[MAXN];
        int_fast32_t rest[MAXN]; // number of rows left
        uint_fast32_t bit0 = start_queens[cnt][0]; // The first queen placed
        uint_fast32_t bit1 = start_queens[cnt][1]; // The second queen placed
        int_fast16_t d = 1; // d is our depth in the backtrack stack
        // The UINT_FAST32_MAX here is used to fill all 'coloumn' bits after n ...
        cols[d] = bit0 | bit1 | (UINT_FAST32_MAX << n);
        // This places the first two queens
        diagl[d] = (bit0 << 2) | (bit1 << 1);
        diagr[d] = (bit0 >> 2) | (bit1 >> 1);
        #define LOOKAHEAD 3
        // we're allready two rows into the field here
        rest[d] = n - 2 - LOOKAHEAD;

        //  The variable posib contains the bitmask of possibilities we still have
        //  to try in a given row ...
        uint_fast32_t posib = (cols[d] | diagl[d] | diagr[d]);
        num=0;
        while (d > 0) {
          // moving the two shifts out of the inner loop slightly improves
          // performance
          uint_fast32_t diagl_shifted = diagl[d] << 1;
          uint_fast32_t diagr_shifted = diagr[d] >> 1;
          int_fast32_t l_rest = rest[d];

          while (posib != UINT_FAST32_MAX) {
            // The standard trick for getting the rightmost bit in the mask
            uint_fast32_t bit = ~posib & (posib + 1);
            uint_fast32_t new_diagl = (bit << 1) | diagl_shifted;
            uint_fast32_t new_diagr = (bit >> 1) | diagr_shifted;
            uint_fast32_t new_posib = (cols[d] | bit | new_diagl | new_diagr);
            posib ^= bit; // Eliminate the tried possibility.
            bit |= cols[d];

            if (new_posib != UINT_FAST32_MAX) {
                uint_fast32_t lookahead = (bit | (new_diagl << (LOOKAHEAD - 2)) | (new_diagr >> (LOOKAHEAD - 2)));
                uint_fast32_t lookahead2 = (bit | (new_diagl << (LOOKAHEAD - 1)) | (new_diagr >> (LOOKAHEAD - 1)));
                uint_fast32_t allowed1 = l_rest >= 0;
                uint_fast32_t allowed2 = l_rest > 0;

                if(allowed1 && (lookahead == UINT_FAST32_MAX)) {
                    continue;
                }

                if(allowed2 && (lookahead2 == UINT_FAST32_MAX)) {
                    continue;
                }

              // The next two lines save stack depth + backtrack operations
              // when we passed the last possibility in a row.
              // Go lower in the stack, avoid branching by writing above the current
              // position
              posibs[d + 1] = posib;
              d += posib != UINT_FAST32_MAX; // avoid branching with this trick
              posib = new_posib;

              l_rest--;

              // make values current
              cols[d] = bit;
              diagl[d] = new_diagl;
              diagr[d] = new_diagr;
              rest[d] = l_rest;
              diagl_shifted = new_diagl << 1;
              diagr_shifted = new_diagr >> 1;
            } else {
              // when all columns are used, we found a solution
              num += bit == UINT_FAST32_MAX;
            }
          }
          posib = posibs[d]; // backtrack ...
          d--;
        }


      TOTAL_SOLUCIONES+=num;
      }
    }



    int fin=-1;
    	while(workersDisponibles<num_workers){
    		MPI_Recv(&buffer,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&estado);
    		workersDisponibles++;
    		if(buffer){
    			// PEDIDO CON SOLUCION
    			TOTAL_SOLUCIONES+=buffer;
    		}

    		MPI_Send(&fin,1,MPI_INT,estado.MPI_SOURCE,0,MPI_COMM_WORLD);
    	}
    return TOTAL_SOLUCIONES*2;
}
void worker(uint_fast8_t n) {


  uint_fast64_t num = 0;
  //
  // The top level is two fors, to save one bit of symmetry in the enumeration
  // by forcing second queen to be AFTER the first queen.
  //
  uint_fast16_t start_cnt = 0;
  uint_fast32_t start_queens[(MAXN - 2)*(MAXN - 1)][2];
  //#pragma omp simd
  for (uint_fast8_t q0 = 0; q0 < n - 2; q0++) {
    for (uint_fast8_t q1 = q0 + 2; q1 < n; q1++) {
      start_queens[start_cnt][0] = 1 << q0;
      start_queens[start_cnt][1] = 1 << q1;
      start_cnt++;
    }
  }
  int tarea=0;
	MPI_Send(&tarea,1,MPI_INT,0,0,MPI_COMM_WORLD);
	MPI_Recv(&tarea,1,MPI_INT,0,0,MPI_COMM_WORLD,&estado);
  while(tarea>-1){
    //Codigo workerito
  for (uint_fast16_t cnt = tarea; cnt < start_cnt; cnt++) {
    uint_fast32_t cols[MAXN], posibs[MAXN]; // Our backtracking 'stack'
    uint_fast32_t diagl[MAXN], diagr[MAXN];
    int_fast32_t rest[MAXN]; // number of rows left
    uint_fast32_t bit0 = start_queens[cnt][0]; // The first queen placed
    uint_fast32_t bit1 = start_queens[cnt][1]; // The second queen placed
    int_fast16_t d = 1; // d is our depth in the backtrack stack
    // The UINT_FAST32_MAX here is used to fill all 'coloumn' bits after n ...
    cols[d] = bit0 | bit1 | (UINT_FAST32_MAX << n);
    // This places the first two queens
    diagl[d] = (bit0 << 2) | (bit1 << 1);
    diagr[d] = (bit0 >> 2) | (bit1 >> 1);
    #define LOOKAHEAD 3
    // we're allready two rows into the field here
    rest[d] = n - 2 - LOOKAHEAD;

    //  The variable posib contains the bitmask of possibilities we still have
    //  to try in a given row ...
    uint_fast32_t posib = (cols[d] | diagl[d] | diagr[d]);

    while (d > 0) {
      // moving the two shifts out of the inner loop slightly improves
      // performance
      uint_fast32_t diagl_shifted = diagl[d] << 1;
      uint_fast32_t diagr_shifted = diagr[d] >> 1;
      int_fast32_t l_rest = rest[d];

      while (posib != UINT_FAST32_MAX) {
        // The standard trick for getting the rightmost bit in the mask
        uint_fast32_t bit = ~posib & (posib + 1);
        uint_fast32_t new_diagl = (bit << 1) | diagl_shifted;
        uint_fast32_t new_diagr = (bit >> 1) | diagr_shifted;
        uint_fast32_t new_posib = (cols[d] | bit | new_diagl | new_diagr);
        posib ^= bit; // Eliminate the tried possibility.
        bit |= cols[d];

        if (new_posib != UINT_FAST32_MAX) {
            uint_fast32_t lookahead = (bit | (new_diagl << (LOOKAHEAD - 2)) | (new_diagr >> (LOOKAHEAD - 2)));
            uint_fast32_t lookahead2 = (bit | (new_diagl << (LOOKAHEAD - 1)) | (new_diagr >> (LOOKAHEAD - 1)));
            uint_fast32_t allowed1 = l_rest >= 0;
            uint_fast32_t allowed2 = l_rest > 0;

            if(allowed1 && (lookahead == UINT_FAST32_MAX)) {
                continue;
            }

            if(allowed2 && (lookahead2 == UINT_FAST32_MAX)) {
                continue;
            }

          // The next two lines save stack depth + backtrack operations
          // when we passed the last possibility in a row.
          // Go lower in the stack, avoid branching by writing above the current
          // position
          posibs[d + 1] = posib;
          d += posib != UINT_FAST32_MAX; // avoid branching with this trick
          posib = new_posib;

          l_rest--;

          // make values current
          cols[d] = bit;
          diagl[d] = new_diagl;
          diagr[d] = new_diagr;
          rest[d] = l_rest;
          diagl_shifted = new_diagl << 1;
          diagr_shifted = new_diagr >> 1;
        } else {
          // when all columns are used, we found a solution
          num += bit == UINT_FAST32_MAX;
        }
      }
      posib = posibs[d]; // backtrack ...
      d--;
    }
  }
  // Devuelve el doble porque es mágico. Acá tiene que enviar el resultado y pedir una nueva tarea
  num*=2;
  MPI_Send(&num,1,MPI_INT,0,0,MPI_COMM_WORLD);
	MPI_Recv(&tarea,1,MPI_INT,0,0,MPI_COMM_WORLD,&estado);
  }
}

// expected results from https://oeis.org/A000170
const uint64_t results[MAXN] = {
    1ULL,
    0ULL,
    0ULL,
    2ULL,
    10ULL,
    4ULL,
    40ULL,
    92ULL,
    352ULL,
    724ULL,
    2680ULL,
    14200ULL,
    73712ULL,
    365596ULL,
    2279184ULL,
    14772512ULL,
    95815104ULL,
    666090624ULL,
    4968057848ULL,
    39029188884ULL,
    314666222712ULL,
    2691008701644ULL,
    24233937684440ULL,
    227514171973736ULL,
    2207893435808352ULL,
    22317699616364044ULL,
    234907967154122528ULL,
    0,  // No se sabe cuanto es N=28 porque se tardó como un año para n=27
    0}; // DE 29 NI HABLEMOS, CINCO VIDAS APROX. A menos que P=NP.

void print_usage() {
  printf("usage: m-queens BOARDSIZE\n");
  printf("   or: m-queens START END\n");
  printf("   or: m-queens -h\n");
  printf("   or: m-queens --help\n");
  printf("\n");
  printf("\
This program computes the number of solutions for the n queens problem\n\
(see https://en.wikipedia.org/wiki/Eight_queens_puzzle) for the board size\n\
BOARDSIZE or for a range of board sizes beginning with START and ending\n\
with END.\n");
  printf("\n");
  printf("options:\n");
  printf("  -h, --help      print this usage information\n");
  printf("\n");
  printf("AUTHOR : sudden6 <sudden6@gmx.at>\n");
  printf("SOURCE : https://github.com/sudden6/m-queens\n");
  printf("LICENSE: GNU General Public License v3.0\n");
}

int main(int argc, char * argv[]) {
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&num_workers);
  MPI_Comm_rank(MPI_COMM_WORLD,&id);
  int start = 1;
  int end = 16; // Testear por defecto

  if (argc == 2) {
    if((strcmp("-h", argv[1]) == 0) || (strcmp("--help", argv[1]) == 0)) {
      print_usage();
      return  EXIT_SUCCESS;
    }

    start = atoi(argv[1]);
    if (start < 2 || start > MAXN) {
      printf("BOARDSIZE must be between 2 and %d!\n", MAXN);
      return EXIT_FAILURE;
    }
    end = start;

  }

  if (argc == 3) {
    start = atoi(argv[1]);
    if (start < 2 || start > MAXN) {
      printf("START must be between 2 and %d\n", MAXN);
      return EXIT_FAILURE;
    }
    end = atoi(argv[2]);
    if (end < start || end > MAXN) {
      printf("END must be between %d and %d\n", start, MAXN);
      return EXIT_FAILURE;
    }
  }

  // properly cast
  uint8_t start8 = (uint8_t) start;
  uint8_t end8 = (uint8_t) end;
  // track if all results passed


    int all_pass = 1;
    if(id==0){

      double time_diff, time_start; // for measuring calculation time
      time_start = get_time();
      uint64_t result;
      result = master(start8);
      time_diff = (get_time() - time_start); // calculating time difference
      // check if result is correct
      //int pass = result == results[n - 1];
      //all_pass &= pass;
      //pass ? printf("PASS ") : printf("FAIL ");
      printf("N %2d, Solutions %18" PRIu64 ", Expected %18" PRIu64
          ", Time %f sec., Solutions/s %f\n",
           start8, result, results[start8 - 1], time_diff, result / time_diff);


    } else {
      worker(start8);
    }
    MPI_Finalize();
    return 0;
    //return all_pass ? EXIT_SUCCESS : EXIT_FAILURE;
}
