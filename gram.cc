#include <string>
#include <iostream>
#include "gram.hh"
#include <ctime>
#include <cstdlib>

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

int hashValue(std::string key, int modulus) {
  int hashValue = 0;
  for (int i = 0; i < key.length(); i++) {
    char c = key[i];
    // Horner's method for computing the value.
    hashValue = (32*hashValue + charToInt(c)) % modulus;
  }
  return hashValue;
}

namespace gram {

  dict* build(void) {
    srand(time(0));
    dict* newD = new dict;
    newD->first = nullptr;
    return newD;
  }
  //get(D, ws):
  // 
  // Takes a dict and a word or bigram and returns a random follower 
  std::string get(dict* D, std::string ws) {
    // create a gram pointer key as nullptr
    gram* key = nullptr;
    // create gram pointer to first entry in dict 
    gram* current = D->first;
    // traverse D until ws is found 
    while (current != nullptr){
      if (current->words == ws){
        // set key pointer to current if ws is found then exit loop 
        key = current;
        break;
      } else {
        current = current->next;
      }
    }
    // if ws is found find a random follower of ws
    if (key != nullptr) {
      int length = key->number;
      int number;
      // compute a random number within range of number of followers of key 
      number = std::rand()%length;
      follower* follower;
      follower = key->followers;
      for (int i = 0; i < number; i++){
        follower = follower->next;
      }
      // return random follower 
      return follower->word;
    }
    // if ws was not found in dict return empty string 
    return ""; 
  }

  std::string get(dict* D, std::string w1, std::string w2) {
    return get(D,w1+" "+w2);
  }
  // add(D, ws, fw):
  // Adds a word or bigram to dict along with its followers
  void add(dict* D, std::string ws, std::string fw) {
    // create var key as a gram pointer 
    gram* key = nullptr;
    // create pointer to first entry in dict 
    gram* current = D->first;
    // create indirect as follower
    gram** indirect = &D->first;
    // traverse dict to find ws 
    while (current != nullptr){
      if (current->words == ws){
        // if ws is found set key pointer to current entry 
        key = current;
        break;
      } 
      // increment indirect and current
      indirect = &current->next;
      current = current->next;
    } 
    // if ws was not found in dict create newKey with correct values 
    if (key == nullptr){
      gram* newKey = new gram;
      newKey->words = ws;
      newKey->number = 0;
      newKey->followers = nullptr;
      newKey->next = nullptr;
      key = newKey;
      *indirect = key;
    }
    // create  pointer to new follower with word as fw 
    follower* newFollower = new follower;
    newFollower->word = fw;
    newFollower->next = nullptr;
    if (key->followers != nullptr){
      // create pointer to followers of key
      follower* currentFollower;
      currentFollower = key->followers;
      // traverse key to end of follwers 
      while (currentFollower->next != nullptr){
        currentFollower = currentFollower->next;
      }
      // add newFollower to end of linked list of follwers 
      currentFollower->next = newFollower;
      // increment number of followers of key
      key->number++;
    } else {
      // if key has no followers add newFollowers to key 
      key->followers = newFollower;
      key->number++;
    }
    //if (D->first == nullptr) ?? what was this did it do anything
    return; 
  }
  
  void add(dict* D, std::string w1, std::string w2, std::string fw) {
     add(D,w1+" "+w2,fw);
  }
     
  void destroy(dict *D) {
    delete D;
  }


} 

