// Aria Killebrew Bruehl Project 1 (with help form the absolutely amazing drop-in tutors)
// freq.cc
//
// This implements the operations for the unordered dictionary data
// structure with the type `freq::dict*` for Project 1 of the Spring
// 2020 offering of CSCI 221.  Specifically, we define the functions
// for a bucket hash table that stores a collection of words and their
// integer counts.
//
// It is described more within the README for this project.
//
// The functions it defines include
//    * `int hashValue(std::string,int)`: give the hash location for a key
//    * `freq::dict* freq::build(int,int)`: build a word count dictionary 
//    * `int freq::totalCount(freq::dict*)`: get the total word count
//    * `int freq::numKeys(freq::dict*)`: get number of words
//    * `void freq::increment(freq::dict*,std::string)`: bump a word's count 
//    * `int freq::getCount(freq::dict*,std::string)`: get the count for a word
//    * `freq::entry* freq::dumpAndDestroy(freq::dict*)`: get the word counts, sorted by frequency
//    * `void freq::rehash(freq::dict*)`: expand the hash table
//
// The top four are implemented already, the other four need to be written.
//

#include <string>
#include <iostream>
#include "freq.hh"
#include <algorithm>


// * * * * * * * * * * * * * * * * * * * * * * *
//
// HELPER FUNCTIONS FOR CHOOSING HASH TABLE SIZE
//

// isPrime(n)
//
// Return whether or not the given integer `n` is prime.
//
bool isPrime(int n) {
  // Handle the obvious cases, including even ones.
  if ((n <= 2) || (n % 2 == 0)) {
    return (n == 2);
  }
  // Try several odd divisors.
  int d = 3;
  while (d*d <= n) {
    if (n % d == 0) {
      // It has a divisor. It's not prime.
      return false;
    }
    d += 2;
  }
  // No divisors. It's prime.
  return true;
}

// primeAtLeast(n)
//
// Return the smallest prime number no smaller
// than `n`.
//
int primeAtLeast(int n) {
  if (n <= 2) {
    return 2;
  }
  int p = 3;
  while (p < n || !isPrime(p)) {
    p += 2;
  }
  return p;
}

// * * * * * * * * * * * * * * * * * * * * * * *
//
// HELPER FUNCTIONS FOR COMPUTING THE HASH VALUE
//

// charToInt(c):
//
// Returns an integer between 0 and 31 for the given character. Pays
// attention only to letters, the contraction quote, "stopper" marks,
// and space.
//
//
int charToInt(char c) {
  if (c >= 'a' && c <= 'z') {
    return c - 'a' + 1;
  } else if (c == '.') {
    return 27;
  } else if (c == '!') {
    return 28;
  } else if (c == '?') {
    return 29;
  } else if (c == '\'') {
    return 30;
  } else if (c == ' ') {
    return 31;
  } else {
    return 0;
  }
}

// hashValue(key,tableWidth):
//
// Returns an integer from 0 to tableWidth-1 for the given string
// `key`. This serves as a hash function for a table of size
// `tableWidth`.  
//
// This method treats the string as a base-32 encoding of the integer
// it computes, modulo `tableWidth`. It relies on `charToInt` defined
// just above.
//
int hashValue(std::string key, int modulus) {
  int hashValue = 0;
  for (int i = 0; i < key.length(); i++) {
    char c = key[i];
    // Horner's method for computing the value.
    hashValue = (32*hashValue + charToInt(c)) % modulus;
  }
  return hashValue;
}


// * * * * * * * * * * * * * * * * * * * * * * *
//
// Operations on freq::dict, and other support functions.
//
namespace freq {

//    * `int hashValue(std::string,int)`: give the hash location for a key
//    * `freq::dict* freq::build(int,int)`: build a word count dictionary 
//    * `int freq::totalCount(freq::dict*)`: get the total word count
//    * `int freq::numKeys(freq::dict*)`: get number of words
//    * `void freq::increment(freq::dict*,std::string)`: bump a word's count 
//    * `int freq::getCount(freq::dict*,std::string)`: get the count for a word
//    * `freq::entry* freq::dumpAndDestroy(freq::dict*)`: get the word counts, sorted by frequency
//    * `void freq::rehash(freq::dict*)`: expand the hash table
  // buildBuckets(howMany):
  //
  // Return an array of buckets of length `howMany`.
  //
  bucket* buildBuckets(int howMany) {
    bucket* bs = new bucket[howMany];
    for (int i=0; i<howMany; i++) {
      bs[i].first = nullptr;
    }
    return bs;
  }

  // build(initialSize,loadFactor):
  //
  // Build a word count dictionary that is roughly the given size, and
  // maintains the given load factor in its hash table.
  //
  dict* build(int initialSize, int loadFactor) {
    dict* newD = new dict;
    newD->numIncrements = 0;
    newD->numEntries    = 0;
    newD->loadFactor    = loadFactor; 
    newD->numBuckets    = primeAtLeast(initialSize);
    newD->buckets       = buildBuckets(newD->numBuckets);
    return newD;
  }

  // numKeys(D):
  //
  // Gives back the number of entries stored in the dictionary `D`.
  //
  int numKeys(dict* D) {
    return D->numEntries;
  }

  // totalCount(D):
  //
  // Gives back the total of the counts of all the entries in `D`.
  //
  int totalCount(dict* D) {
    return D->numIncrements;
  }


  // getCount(D,w):
  //
  // Gets the count associated with the word `w` in `D`.
  //
  int getCount(dict* D, std::string w) {
    // compute the hash value
    int hash = hashValue(w, D->numBuckets);
    // pointer to the first node of the dict 
    entry* currentNode = D->buckets[hash].first;
    // traverse the dictionary 
    while (currentNode != nullptr) {
      if (currentNode->word == w) {
        // return the count of w if found 
        return currentNode->count;
      } else {
        currentNode = currentNode->next;
      }
    }
    // if w not found return 0 as count 
    return 0;
  }

  // rehash(D):
  //
  // Roughly doubles the hash table of `D` and places its entries into
  // that new structure.
  //
  void rehash(dict* D) {
    // compute the number of buckets for the new array as being the prime at least twice as large as the last number of buckets  
    int numBuckets = primeAtLeast(D->numBuckets * 2 + 1);
    // create a pointer to new buckets 
    bucket* buckets = buildBuckets(numBuckets);
    // loop through array of buckets 
    for (int i = 0; i < D->numBuckets; i++) { 
      // create pointer to first bucket in dict 
      entry* currentNode = D->buckets[i].first;
      // traverse entries of bucket 
      while (currentNode != nullptr) {
        // use indirect to traverse entries in bucket 
        entry** indirect = &buckets[hashValue(currentNode->word, numBuckets)].first;
        while (*indirect != nullptr) {
          indirect = &(*indirect)->next;
        }
         // increment currentNode using indirect
        *indirect = currentNode;
        entry* next = currentNode->next;
        currentNode->next = nullptr;
        currentNode = next;
      }
    }
    // delete old buckets 
    delete [] D->buckets;
    // set dict's buckets to the new array of buckets 
    D->buckets = buckets;
    D->numBuckets = numBuckets;
    return;
  }

  // increment(D,w):
  //
  // Adds one to the count associated with word `w` in `D`, possibly
  // creating a new entry.
  //
  void increment(dict* D, std::string w) {
    // increment the number of entreris in the dict 
    D->numIncrements++; 
    // compute the hash value 
    int hash = hashValue(w, D->numBuckets);
    // pointer to the first node of the dictionary 
    entry* currentNode = D->buckets[hash].first;
    // traverse the dictionary to find w 
    while (currentNode != nullptr) {
      if (currentNode->word == w) {
        // increment the count of w if found 
        currentNode->count ++;
        return;
      } else {
        currentNode = currentNode->next;
      }
    }
    // compute the load factor 
    int loadFactor = (D->numEntries + 1)/D->numBuckets; 
    // rehash if necessary 
    if (loadFactor == D->loadFactor) {
      rehash(D);
    }
    // create a newEntry node with appropriate values 
    entry* newEntry = new entry;
    newEntry->word = w;
    newEntry->count = 1;
    newEntry->next = nullptr;
    D->numEntries++;
    // use indirect to traverse buckets and add newEntry to the end 
    entry** indirect = &D->buckets[hash].first;
    while (*indirect != nullptr) {
      indirect = &(*indirect)->next;
    }
    *indirect = newEntry;
  }

  //
  // Takes pointer to an array of size length and sorts the array using insertion sort
  //
  void sort(entry* array, int length) {
  // traverse array until end 
  for (int i = 0; i < length; i++) {
    // traverse backward through entries up to position i 
    for (int j = i; j > 0 && array[j].count < array[j - 1].count; j--) {
      // check if entries in array need to be swapped 
      if (array[j].count < array[j - 1].count) {
        // create temp entry to hold entry at position j 
        entry temp = array[j];
        // assign value of entry at position j to be value of entry at position j-1
        array[j] = array[j - 1];
        // set value of entry at position j-1 to be valeu of temp entry 
        array[j - 1] = temp;
      }
    }
  }
  // reverse array so it is sorted in order of decreasing count 
  std::reverse(array, array + length);
  }

  // dumpAndDestroy(D):
  //
  // Return an array of all the entries stored in `D`, sorted from
  // most to least frequent.
  //
  // Deletes all the heap-allocated components of `D`.
  //
  entry* dumpAndDestroy(dict* D, int* sizep) {
    // create length var to keep track of entries in dict
    int length = 0;
    // create new array of entries of length numEntries 
    entry* newArray = new entry[D->numEntries];
    // loop through process of XX for each bucket 
    for (int i = 0; i < D->numBuckets; i++) {
      // loop through each entry of a bucket 
      for (entry* currentNode = D->buckets[i].first; currentNode != nullptr; currentNode = currentNode->next) {
        // set word for entry at poition length 
        newArray[length].word = currentNode->word;
        // set count for entry at position length 
        newArray[length].count = currentNode->count;
        // set next for entry at position length to be nullptr 
        newArray[length].next = nullptr;
        length++;
      }
    } 
    // set sizep pointer to have value length 
    *sizep = length;
    // sort the newArray 
    sort(newArray, length);
    return newArray;
    
    // UNIMPLEMENTED
    /*entry* es = new entry[3];
    es[0].word = "hello";
    es[0].count = 0;
    es[0].next = nullptr;
    es[1].word = "world";
    es[1].count = 1;
    es[1].next = nullptr;
    es[2].word = "!";
    es[2].count = 2;
    es[2].next = nullptr;
    return es;*/
  }
} // end namespace freq

