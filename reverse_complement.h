#ifndef REVERSE_COMPLEMENT_H
#define REVERSE_COMPLEMENT_H

/**
 *Make sure out is allocated with the proper amount of space
 * len(out) = strlen(in) + 1
 *@param end like c++ iteratores end should be one past the last letter you want to reverse complement
 *@return 1 for success, 0 failure
 */
int reverseComplement(const char *begin, const char *end, char *out);


#endif
