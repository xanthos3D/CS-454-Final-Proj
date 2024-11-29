#include <iostream>
#include <string>
#include <gmpxx.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <tuple>
using namespace std;
//run code with
//g++ p1.cpp -lgmp -lgmpxx
//./a.out

//int value for the one failing state in the dfa
static long FAIL_STATE = 1365;
const int numberOfStates = 1365; // 4^5 + 4^4 + 4^3 + 4^2 + 4^1 + 1 (for empty string)
const int bufferLength = 5; //Default buffer length
const char characters[] = {'a', 'b', 'c', 'd'}; //Initialize the alphabet
int DFAStates[numberOfStates];

// Encoding a buffer for right to left using the formula below
// (charRepresentation) * 4^4 + (charRepresentation) * 4^3 + (charRepresentation) * 4^2 +
//                       (charRepresentation) * 4^2 + (charRepresentation) * 4^1 + (charRepresentation) * 4^0
// Precondition: currentBuffer contains only characters from {'a', 'b', 'c', 'd'}.
// Postcondition: Returns an integer representing the base-4 encoded value of the buffer.
// Input: currentBuffer - a string consisting of characters 'a', 'b', 'c', or 'd'.
// Output: Returns an integer representing the base-4 encoding of currentBuffer.
int encode (std::string currentBuffer) {
    int encodedSum = 0;

    for (int i = currentBuffer.length() - 1; i >= 0; i--) {
        int multiple;

        switch (currentBuffer.at(i)) {
            case 'a': multiple = 1;
                break;
            case 'b': multiple = 2;
                break;
            case 'c': multiple = 3;
                break;
            case 'd': multiple = 4;
                break;
        }

        encodedSum += pow(4, currentBuffer.length() - 1 - i) * multiple;
    }
    return encodedSum;
}

// Decode a buffer from the lowest bits to the highest using base-4 representation.
// Precondition: buffer is a non-negative integer representing a valid encoded state.
// Postcondition: Returns a string decoded from the buffer, containing only characters from {'a', 'b', 'c', 'd'}.
// Input: buffer - an integer representing the encoded state of a string.
// Output: Returns a string decoded from the buffer, consisting of characters 'a', 'b', 'c', or 'd'.
std::string decode(int buffer) {
    std::string decodedBuffer;
    char nextChar;
    int base = 4;

    // Loop until the buffer becomes 0
    while (buffer > 0) {

        int lastChar = buffer % base;

        if (lastChar == 0) {
            buffer = (buffer / base) - 1;
        } else {
            buffer /= base;
        }

        switch (lastChar) {
            case 1: nextChar = 'a'; break;
            case 2: nextChar = 'b'; break;
            case 3: nextChar = 'c'; break;
            case 0: nextChar = 'd'; break;
        }

        // Prepend the character to the decodedBuffer
        decodedBuffer = nextChar + decodedBuffer;
    }

    return decodedBuffer;
}

// Counts the number of accepting transitions for a given buffer state.
// Precondition: buffer is a non-negative integer representing a valid state; prev and next are vectors of size numberOfStates with initialized GMP integer types (mpz_t).
// Postcondition: Updates the next vector with the count of accepting transitions from the given buffer state.
// Input: buffer - an integer representing a state; prev - vector of GMP integer types with counts for the previous length; next - vector of GMP integer types to store updated counts.
// Output: The next vector is updated with the number of accepting transitions from the current buffer state.
void countAccepting(int buffer, std::vector<std::tuple<mpz_t,int>>& prev, std::vector<std::tuple<mpz_t,int>>& next) {
    std::string bufferHolder = decode(buffer);

    // Create a 6 character substring with buffer + each character
    for (char c: characters) {
        std::string currentSubString = bufferHolder + c;

        // BitMask for occurrences of a,b,c,d
        int valid = 0;

        int Atraversed = 0;

        // For each character a,b,c,d seen logical(or) a specific bit of valid
        for (char i: currentSubString) {

            switch (i) {
                case 'a':

                    //if we see an a then increas as such.
                    Atraversed++;
                    std::cout<<"Atraversed: "<<Atraversed<<std::endl;
                    valid = valid | 0b0001;
                    break;
                case 'b':
                    valid = valid | 0b0010;
                    break;
                case 'c':
                    valid = valid | 0b0100;
                    break;
                case 'd':
                    valid = valid | 0b1000;
                    break;
            }
        }

        // All Substrings of less than  length 6 are valid, also need to update the A's scene
        if (currentSubString.length() < 6) {

            int acceptedTran = encode(currentSubString);
            std::get<1>(next[buffer]) = std::get<1>(prev[acceptedTran]) + Atraversed;
            mpz_add(std::get<0>(next[buffer]), std::get<0>(next[buffer]), std::get<0>(prev[acceptedTran])); //add prev into next using gmp
        }else if (valid == 15) { // 15 = 0b1111 which represents all a,b,c,d being seen

            std::string temp = currentSubString.substr(1);
            int acceptedTran = encode(temp);

            std::get<1>(next[buffer]) = std::get<1>(prev[acceptedTran]) + Atraversed;
            mpz_add(std::get<0>(next[buffer]), std::get<0>(next[buffer]), std::get<0>(prev[acceptedTran]));//add prev into next using gmp
        }
    Atraversed = 0;
    }
}

// Recursively checks each DFA state's transitions for all strings of length 1 through n.
// Precondition: i is a positive integer representing the current string length, and n is the maximum string length. prev and next are vectors of size numberOfStates with initialized GMP integer types (mpz_t).
// Postcondition: Computes the number of valid transitions for strings up to length n and updates the prev and next vectors.
// Input: i - current string length; n - target maximum string length; prev - vector storing transition counts for length i-1; next - vector storing transition counts for length i.
// Output: Updates prev and next vectors with the counts for the transitions of strings up to length n.
void countNumOfAcceptHelp(int i,int j, int n,int m, std::vector<std::tuple<mpz_t,int>>& prev, std::vector<std::tuple<mpz_t,int>>& next) {

    // Base case: the length i string has reached our max length n
    if (i == n) {
        // Check the DFAStates one last time for the final length string
        for (int DFAState : DFAStates) {
            //int buffer,int As,int m,
            countAccepting(DFAState, prev, next);
        }
        return;

    }

    // Check DFAStates for the total number of validTransitions

    for (int DFAState : DFAStates) {
        //modify the count Accepting so that it takes into account a's
        countAccepting(DFAState, prev, next);
    }

    // All DFAStates have been checked for current length so copy the vectors
    // and initialize next[] with zeros
    for (size_t i = 0; i < numberOfStates; ++i) {
        mpz_init(std::get<0>(prev[i]));
        mpz_set(std::get<0>(prev[i]), std::get<0>(next[i]));
        mpz_set_ui(std::get<0>(next[i]), 0);
    }

    // Recursive step: Increase the size of the string being tested
    countNumOfAcceptHelp(i + 1,j, m , n, prev, next);
}



// Starts the recursive call chain and reports the results.
// Precondition: n is a non-negative integer representing the length of strings to consider. result is an initialized GMP integer type (mpz_t).
// Postcondition: Computes the number of strings of length n that are accepted by the DFA and stores the result in the provided mpz_t variable.
// Input: n - the length of the strings to be considered; result - a GMP integer to store the final count of accepting strings.
// Output: Updates result to contain the number of accepting strings of length n.
void countNumOfAccept(int n,int m, mpz_t result) {

    //make a tuple to store the number of a's and states,
    std::vector<std::tuple<mpz_t,int>> prev(numberOfStates); //  A vector of GMP integer types (mpz_t)
    std::vector<std::tuple<mpz_t,int>> next(numberOfStates); //  A vector of GMP integer types (mpz_t)

    // Initializes all indexes to 1
    // For a string of length 0 every state is an accepting state
    //however this is not the case depending on if m is 0 or not.

    //if m is zero then every state at length zero is accepting, otherwise not
    for (int i = 0; i < numberOfStates; ++i) {

        mpz_init(std::get<0>(prev[i])); // Initialize the index to a GMP integer types (mpz_t)
        mpz_init(std::get<0>(next[i])); // Initialize the index to a GMP integer types (mpz_t)

        //in the tuple set the A's seen to zero
        std::get<1>(prev[i]) = 0;
        std::get<1>(prev[i]) = 0;

        //if m = 0 then strings of length zero are all accepting
        if(m == 0){
            mpz_set_ui(std::get<0>(prev[i]), 1); // Set each index to 1
        
        //otherwise if m is greater than zero, strings of length zero are not accepting.
        }else{
         mpz_set_ui(std::get<0>(prev[i]), 0); // Set each index to 1   
        }
        mpz_set_ui(std::get<0>(next[i]), 0); // set each index to 0
    }

    // Start recursive call chain at string of length 1 since prev is
    // already populated for a string of length 0
    countNumOfAcceptHelp(1, 0, n, m, prev, next);

    mpz_set(result, std::get<0>(next[0]));

}

// Recursively generates all possible states of length 0 to 5 for the characters a, b, c, d.
// Precondition: length is a non-negative integer indicating the desired length of the generated string.
// Postcondition: Updates the DFAStates array to include all encoded states of the specified lengths.
// Input: currentBuffer - the current string buffer being generated; length - the remaining length to generate.
// Output: The DFAStates array is updated to include all states representing strings of length 1 to 5.
void generateHelper(const std::string& currentBuffer, int length) {
    // Base case: when the string has reached the desired length
    // Map the encoded state and its encoded transitions
    if (length == 0) {
        //std::cout << currentBuffer << std::endl;
        int stateID = encode(currentBuffer);
        // If this state has not been seen, initialize its transition map
        if (DFAStates[stateID] == 0) {
            DFAStates[stateID] = stateID;
        }
        return;
    }
    // Recursive call: generate strings up to the specified length
    for (char c : characters) { // Iterate through the character set
        generateHelper(currentBuffer + c, length - 1); // Append character and recurse
    }
}

// Calls the recursive helper to generate all possible DFA states for lengths up to 5.
// Precondition: None.
// Postcondition: The DFAStates array is populated with all possible encoded states up to the specified buffer length.
// Input: None.
// Output: The DFAStates array is updated to include all possible states of lengths 1 to 5.
void generateDFAStates() {
    for (int length = 1; length <= bufferLength; length++) {
        generateHelper("", length); // Start with an empty string for each length
    }
}

/** **************************************************************************************
driver of our program
*****************************************************************************************/
int main() {

   generateDFAStates();
    int n;
    bool inputValid = false;
    mpz_t result;
    mpz_init(result);

    for (int i = 0; i < 20; ++i) {
        std::cout<<"====================================================="<<std::endl;
        for (int j = 0; j < 7; ++j) {
            countNumOfAccept(i,j, result);
            std::cout << "For a string of size n = " << i ;
            std::cout << " with m A's = " << j ;
            std::cout << " the total number of accepting strings: ";
            gmp_printf("%Zd\n", result);
        
        }
      
    }

    /*while(!inputValid){
        std::cout << "Enter the length of the strings n > 0 and n <= 300:  ";
        std::cin >> n;
        if(n > 0 && n <= 300) {
            countNumOfAccept(n, result);
            inputValid = true;
        } else{
            std::cout << "Invalid input!" << std::endl;
        }
    }
    std::cout << "For a string of size n = " << n ;
    std::cout << " the total number of accepting strings: ";
    gmp_printf("%Zd\n", result);*/
    return 0;
}

