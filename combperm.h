/**
 * COMBINATIONS AND PERMUTATIONS
 * Freeware. No pull requests accepted.
 * Made by: E. Dronkert, Utrecht NL, May 2024.
 * https://github.com/ednl
 * https://mastodon.social/@ednl
 */

#ifndef COMBPERM_H
#define COMBPERM_H

// Successive calls give combinations of k indices from a set of n.
// Adapted from Knuth 4A, §7.2.1.3, algorithm T.
// Returns pointer to array of int, index 0..k-1.
// For example: combinations(3,2) gives [0,1], [0,2], [1,2]
// Call as combinations(0,0) to reset and free memory.
extern int *combinations(const int n, const int k);

// Successive calls give permutations in lexicographic order of 'count' index numbers.
// Ref.: https://en.wikipedia.org/wiki/Permutation#Generation_in_lexicographic_order
// Returns pointer to first element of next permutation of 'count' index numbers.
// Returns NULL (and memory is freed) when all permutations have been visited.
//   Set count<=0 to free memory.
//   NB: not thread-safe because permutation and state
//       are stored in local static variables.
extern int *permutations(const int count);

// Successive calls give permutations in "plain changes" order of 'count' index numbers.
// Permutations in the second half are lexically reversed from those in the first half.
// Ref.: https://en.wikipedia.org/wiki/Steinhaus–Johnson–Trotter_algorithm
//       with improvement by Shimon Even as implemented by:
//       https://github.com/steppi/adeft/blob/master/src/adeft/score/permutations.pyx
// Returns pointer to first element of next permutation of 'count' index numbers.
// Returns NULL (but memory NOT freed) when all permutations have been visited.
//   Set count<=0 to free memory.
//   NB: not thread-safe because permutation and state
//       are stored in local static variables.
extern int *plainchanges(const int count);

#endif // COMBPERM_H
