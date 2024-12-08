#include <iostream>
#include <cmath>
#include <gmpxx.h>
#include <vector>




/*
 * CS 454 Fall 2024
 * Final Project
 * Problem #3A *INCORRECT IMPLEMENTATION*
 * Group Members:
 * Richard Yocke
 * Xander Maniaci
 * Nathan Walker
 *
 * This program was complied on windows using the included cmake file,
 * or on the blue server using the following commands:
 * g++ -o finalProject_3A_Incorrect finalProject_3A_correct.cpp -lgmpxx -lgmp
 * ./finalProject_3A_correct
 *
 * */

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
int encode(std::string currentBuffer) {
    int encodedSum = 0;

    for (int i = currentBuffer.length() - 1; i >= 0; i--) {
        int multiple;

        switch (currentBuffer.at(i)) {
            case 'a':
                multiple = 1;
                break;
            case 'b':
                multiple = 2;
                break;
            case 'c':
                multiple = 3;
                break;
            case 'd':
                multiple = 4;
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
            case 1:
                nextChar = 'a';
                break;
            case 2:
                nextChar = 'b';
                break;
            case 3:
                nextChar = 'c';
                break;
            case 0:
                nextChar = 'd';
                break;
        }

        // Prepend the character to the decodedBuffer
        decodedBuffer = nextChar + decodedBuffer;
    }

    return decodedBuffer;
}


// Recursively generates all possible states of length 0 to 5 for the characters a, b, c, d.
// Precondition: length is a non-negative integer indicating the desired length of the generated string.
// Postcondition: Updates the DFAStates array to include all encoded states of the specified lengths.
// Input: currentBuffer - the current string buffer being generated; length - the remaining length to generate.
// Output: The DFAStates array is updated to include all states representing strings of length 1 to 5.
void generateHelper(const std::string &currentBuffer, int length) {
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
    for (char c: characters) { // Iterate through the character set
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


// Counts the number of accepting transitions for a given buffer state.
// Precondition: buffer is a non-negative integer representing a valid state.
// Precondition: c is a char {a, b, c, d}
// Input: buffer current five character state whose transitions are being considered.
// Input: c is the current character transition.
// Post-condition: Updates the next vector with the count of accepting transitions from the given
//                  buffer state and the number of a's seen.


void checkForCharacters(int buffer, char c, std::vector<mpz_class> &prev, std::vector<mpz_class> &next) {
    std::string bufferHolder = decode(buffer);
    std::string currentSubString = bufferHolder + c;

    // BitMask for occurrences of a,b,c,d
    int valid = 0;

    // For each character a,b,c,d seen logical(or) a specific bit of valid
    for (char i: currentSubString) {
        switch (i) {
            case 'a':
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

    // All Substrings of less thand length 6 are valid
    if (currentSubString.length() < 6) {
        int acceptedTran = encode(currentSubString);
        next[buffer] += prev[acceptedTran]; //add prev into next using gmp
    } else if (valid == 15) { // 15 = 0b1111 which represents all a,b,c,d being seen
        std::string temp = currentSubString.substr(1);
        int acceptedTran = encode(temp);
        next[buffer] += prev[acceptedTran];//add prev into next using gmp
    }
}

/** **************************************************************************************
if we have a value below or equal to 5 for strings with a in the middle, then simply search
for the number of strings which have a's in the middle, since all stings below 5 where accepted
in our original language.
@pre:taken in a n <= 5
@post: returns number of strings with a's in the middle
*****************************************************************************************/
 mpz_class fiveOrBelow(int size){

        int stringsWithAsInTtheMiddle = 0;
        
        //want to loop through our length 
        //for(int length = 1; length <= size; length++ ){
            int length = size;
            //std::cout<<length<<std::endl;
            //if we have a odd number and that number isnt 1
            if(length != 1){
                
                // strings with a in the middle can be generated by taking 4^length-1 then adding 1 to resemble the a.
                stringsWithAsInTtheMiddle = pow(length-1, 4); 

            //otherwise if length = 1
            }else if(length == 1){

                std::cout<<'a'<<std::endl;
                // add one, since the only valid string with a's in the middle is 
                stringsWithAsInTtheMiddle++;
            }

        return stringsWithAsInTtheMiddle;
    
}


bool isValid(int buffer, char c) {
    std::string bufferHolder = decode(buffer);
    std::string currentSubString = bufferHolder + c;

    // BitMask for occurrences of a,b,c,d
    int valid = 0;

    // For each character a,b,c,d seen logical(or) a specific bit of valid
    for (char i: currentSubString) {
        switch (i) {
            case 'a':
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
    if (valid == 15) {
        return true;
    } else {
        return false;
    }
}

// This function implements an incorrect algorithm which counts strings up to n/2 and then checks each state for its
// transition on a and then continues to count the second half of n from there. This makes a flawed assumption about how
// prev and next the number of accepted strings of a certain length.
// Precondition: i is a positive integer representing the current string length, and n is the maximum string length. prev and next are vectors of size numberOfStates with initialized GMP integer types (mpz_t).
// Post-condition: Computes the number of valid transitions for strings up to length n and updates the prev and next vectors.
// Input: i - current string length; n - target maximum string length; prev - vector storing transition counts for length i-1; next - vector storing transition counts for length i.
// Output: Updates prev and next vectors with the counts for the transitions of strings up to length n.
void countNumOfAcceptHelp(int i, int n, mpz_class &sum, std::vector<mpz_class> &prev, std::vector<mpz_class> &next) {

    // Base case: the length i string has reached our max length n
    if (i >n) {
        // Check the DFAStates one last time for the final length string
        return;
    }

    // Check DFAStates for the total number of validTransitions
    for (int DFAState: DFAStates) {
            // Create a 6 character substring with buffer + each character
            for (char c: characters) {
                checkForCharacters(DFAState, c, prev, next);
            }
    }

    // All DFAStates have been checked for current length so copy the vectors
    // and initialize next[] with zeros
    for (size_t k = 0; k < numberOfStates; ++k) {
        prev[k] = next[k];
        next[k] = 0;
    }
    // Recursive step: Increase the size of the string being tested
    countNumOfAcceptHelp(i + 1, n, sum, prev, next);
}


// Starts the recursive call chain and reports the results.
// Precondition: n is a non-negative integer representing the length of strings to consider. result is an initialized GMP integer type (mpz_t).
// Post-condition: Computes the number of strings of length n that are accepted by the DFA and stores the result in the provided mpz_t variable.
// Input: n - the length of the strings to be considered; result - a GMP integer to store the final count of accepting strings.
// Output: Updates result to contain the number of accepting strings of length n.
void countNumOfAccept(int length, mpz_class &result) {

    std::vector<mpz_class> prev(numberOfStates); //  A vector of GMP integer types (mpz_t)
    std::vector<mpz_class> next(numberOfStates); //  A vector of GMP integer types (mpz_t)

    // throw if given a value that is even
    if (length % 2 == 0) {

        std::cout<<"cant find strings with a's in the middle, if there is no middle."<<std::endl;
        //std::cout<<"N = "<<length<<std::endl;
        throw;

    }

    //if n is less then 5 our task becomes very easy. we simply need to check if the middle character is an a.

    if (length <= 5) {
        // call our function to get number of strings with a's in the middle.
        result = fiveOrBelow(length);
        return;
    }

    int n = length-4;
    //std::cout<<"n: "<<n<<std::endl;

    // Initializes all indexes to 1
    // For a string of length 0 every state is an accepting state
    for (int i = 0; i < numberOfStates; ++i) {
        prev[i] = 1; // Set each index to 1
        next[i] = 0; // set each index to 0
    }
    mpz_class sum = 0;
    // Start recursive call chain at string of length 1 since prev is
    // already populated for a string of length 0
    countNumOfAcceptHelp(3, n, sum, prev, next);
    //result = next[0];

    for (size_t i = 0; i < numberOfStates; ++i) {
        if (decode(DFAStates[i]).length() == 5) {
            if (isValid(DFAStates[i], 'a')){
                std::string temp = decode(DFAStates[i]).substr(1) + 'a';
                int tempState = encode(temp);
                sum += prev[tempState];
            }
        }
    }

    result = sum;

}


int main() {
    generateDFAStates();
    int n;
    bool inputValid = false;
    mpz_class result;
    /*
    while (!inputValid) {
        std::cout << "Enter an Odd length of the strings n > 0 and n <= 300:  ";
        std::cin >> n;
        if (n > 6 && n <= 300 && n % 2 == 1) {
            countNumOfAccept(n / 2, result);
            inputValid = true;
        } else {
            std::cout << "Invalid input!" << std::endl;
        }
    }
    std::cout << "For a string of size n = " << n;
    std::cout << " the total number of accepting strings: ";
    std::cout << result;*/

    int lengths = 11;
    std::cout<< "STRINGS WITH A'S IN THE MIDDLE"<<std::endl;
    std::cout<< "++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
    for (int i = 1; i < lengths; i = i = i+2) {
         std::cout<< "result for "<<i;
         countNumOfAccept(i, result);
         std::cout<<" : " << result <<std::endl;
      
    }
    return 0;
}
