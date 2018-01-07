#include <math.h>

//Gets the bits in num between lower and upper, inclusive
//0 1 2 3 4 5 6 7
//TODO: Reverse indexing to match up with convention
unsigned char get_bits(unsigned char num, int lower, int upper) {
    return (num % (int)pow(2,7-lower+1) ) / ((int)pow(2,7-upper));
}
