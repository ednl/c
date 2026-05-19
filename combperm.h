#ifndef COMBPERM_H
#define COMBPERM_H

// Successive calls give combinations of k indices from a set of n.
// Adapted from Knuth 4A, §7.2.1.3, algorithm T.
// Returns pointer to array of int, index 0..k-1.
// For example: combinations(3,2) gives [0,1], [0,2], [1,2]
// Call as combinations(0,0) to reset and free memory.
extern int *combinations(const int n, const int k);

// Successive calls give permutations in lexicographic order of len index numbers.
// https://en.wikipedia.org/wiki/Permutation#Generation_in_lexicographic_order
// NB: not thread-safe because permutations are stored in local static variable.
// Returns pointer to first element of next permutation of len index numbers.
// Returns NULL (and memory is freed) when all permutations have been visited.
// Call as permutations(0) to manually reset and free memory.
extern int *permutations(const int len);

// Successive calls give permutations in "plain changes" order of len index numbers.
// https://en.wikipedia.org/wiki/Steinhaus–Johnson–Trotter_algorithm
//   with improvement by Shimon Even, as implemented by:
//   https://github.com/steppi/adeft/blob/master/src/adeft/score/permutations.pyx
// Second half of permutations are lexically reversed from first half:
//   perm(i) = reverse(perm(n!/2 + i)) for i=0..(n!/2)-1
// NB: not thread-safe because permutation and state are stored in local static variables.
// Returns pointer to first element of next permutation of len index numbers.
// Returns NULL (and memory is freed) when all permutations have been visited.
// Call as plainchanges(0) to manually reset and free memory.
extern int *plainchanges(const int len);

#endif // COMBPERM_H
