// Aria Killebrew Bruehl Project 1 (with help from the amazing drop-in tutors!)
#include <iostream>
#include "gram.hh"

// next_word_in(line):
//
// Process the characters of the std::string object 'line', seeking
// the next contiguous sequence of letters within that string.
//
// Once it has processed that next word, it modifies 'line' to exclude
// the prefix of processed characters from it, and returns that next
// word as a string. The word will be a string of lowercase alphabetic
// characters.
//
// It returns the empty string once the line has been fully processed.
//
std::string next_word_in(std::string &line) {
  std::string word = "";
  int i;
  for (i=0; i < line.size(); i++) {
    char c = line[i];

    // Make letters lowercase.
    if ('A' <= c && c <= 'Z') {
      c = c+32;
    }

    // Include letters or contraction marks as part of a word.
    if (('a' <= c && c <= 'z') || c == '\'') {
      word += c;

    // If we hit a "stopper", emit it as a word.   
    } else if (c == '.' || c == '!' || c == '?') {
      if (word.size() > 0) {
	line.erase(0,i);
	return word;
      } else {
	word += c;
	line.erase(0,1);
	return word;
      }

    // If we hit any non-letter, emit a word.
    } else {
      if (word.size() > 0) {
	line.erase(0,i);
	return word;
      }
    }
  }
  // End of the line, emit that ending word.
  line.erase(0,i);
  return word;
}

// train_chat():
//
// Returns a new dictionary of word/bigram followers built using the
// structure of the text from `std::cin`.
//
gram::dict* train_chat(void) {
  gram::dict *d = gram::build();
  std::string line;
  std::string w1 = ".";
  std::string w2 = next_word_in(line);
  std::string w  = "";
  // Read until the end of text entry.
  while (std::cin || line != "") {
    if (line == "") {
      std::getline(std::cin,line);
    }
    w = next_word_in(line);
    if (w != "") {
      gram::add(d,w1,w2,w); // Add a follower `w` for the bigram words `w1` and `w2`.
      gram::add(d,w1,w2);   // Add a follower `w` for the word `w2`.
      w1 = w2;
      w2 = w;
    }
  }
  gram::add(d,w1,w2);       // Add the last word as a follower.
  gram::add(d,w1,w2,".");   // Include the last word as preceding a stopper `.`.
  return d;
}

// chat(d, lineWidth, numLines):
//
// Using the text-trained word/bigram dictionary of followers,
// generate a random text with `numLines`, where each line is
// no longer than `lineWidth` characters.
//
void chat(gram::dict* d, int lineWidth, int numLines) {
  // assume first oldWord is "." to endsure sentence starts logically
  std::string oldWord = ".";
  // use get to find a random follower word to start sentence 
  std::string newWord = get(d, oldWord);
  // loop through process of outputting a line numLines times
  for (int lines = 0; lines < numLines; lines++) {
    // create var totalLength to track length of line
    int totalLength = 0;
    // bool var addSpace to indicate adding space before word starts a false for first word of each line
    bool addSpace = false;
    while (true){
      // compute length of new word and increment totalLength
      int wordLength = newWord.length();
      totalLength += wordLength;
      // if totalLength becomes too long break out of while loop
      if (totalLength > lineWidth) {
        // if on last line end with "." 
        if (lines == numLines - 1){
          std::cout << ".";
        }
        break;
      }
      // if newWord is punctuation addSpace becomes false 
      if (newWord == "." || newWord == "!" || newWord == "?"){
        addSpace = false;
      }
      // if addSpace is true print a space before newWord
      if (addSpace){
        std::cout << " ";
        // increment totalLength
        totalLength++;
      }
      // print out newWord to console
      std::cout << newWord;
      // set oldWord to be word just printed
      oldWord = newWord;
      // find newWord to be a random follower of oldWord
      newWord = get(d, oldWord);
      // set addSpace to true
      addSpace = true;
    }
    // create new line at the end once lines has reached maximum length
    std::cout << std::endl;
  }
}

// main()
//
// Processes std::cin as a sequence of words, training a 
// random process based on its bigrams and trigrams, 
// as specified in a `gram::dict`.  
//
// Generates a random text using that process' `gram::dict`
//
int main(int argc, char **argv) {

  //
  // Build a dictionary of word/bigram followers based on the text entered.
  std::cout << "READING text from STDIN. Hit ctrl-d when done entering text.\n";

  gram::dict* d = train_chat();
  chat(d,60,20);
}
    

