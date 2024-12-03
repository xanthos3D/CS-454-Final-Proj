#include <iostream>
#include <cmath>
#include <gmpxx.h>
#include <vector>
#include <iomanip>

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

//g++ p1.cpp -lgmp -lgmpxx
//./a.out

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
// Precondition: buffer is a non-negative integer representing a valid state; prev and next are vectors of size numberOfStates with initialized GMP integer types (mpz_t).
// Postcondition: Updates the next vector with the count of accepting transitions from the given buffer state.
// Input: buffer - an integer representing a state; prev - vector of GMP integer types with counts for the previous length; next - vector of GMP integer types to store updated counts.
// Output: The next vector is updated with the number of accepting transitions from the current buffer state.
void
countNumOfAcceptTransition(int n, int m, int a, int buffer, char c, mpz_class prev[][1365], mpz_class next[][1365]) {
    std::string bufferHolder = decode(buffer);

    // Create a 6 character substring with buffer + c
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
        //std::cout << "buffer" << buffer << std::endl;

        int acceptedTran = encode(currentSubString);
        //std::cout << "acceptedTran" << acceptedTran << std::endl;
        if (c == 'a') {
            //std::cout << a << std::endl;
            if (a + 1 <= m) {
                next[a+1][buffer] += prev[a][acceptedTran];
                //std::cout << "Next with an a" << next[a+1][buffer]<< std::endl;
            }

        } else {
            //std::cout << a << std::endl;
            next[a][buffer] += prev[a][acceptedTran];
            //std::cout << "Next with an anything else" << next[a][buffer]<< std::endl;
        }
        //std::cout << "number of a's " << a << " current string " << currentSubString << std::endl;
    } else if (valid == 15) { // 15 = 0b1111 which represents all a,b,c,d being seen

        std::string temp = currentSubString.substr(1);
        int acceptedTran = encode(temp);

        if (c == 'a') {
            //std::cout << a << std::endl;
            if (a + 1 <= m) {
                next[a+1][buffer] += prev[a][acceptedTran];
            }
        } else {
            //std::cout << a << std::endl;
            next[a][buffer] += prev[a][acceptedTran];
        }
        //std::cout << "number of a's " << a << " current string " << temp << std::endl;
    }

}


// Recursively checks each DFA state's transitions for all strings of length 1 through n.
// Precondition: i is a positive integer representing the current string length, and n is the maximum string length. prev and next are vectors of size numberOfStates with initialized GMP integer types (mpz_t).
// Postcondition: Computes the number of valid transitions for strings up to length n and updates the prev and next vectors.
// Input: i - current string length; n - target maximum string length; prev - vector storing transition counts for length i-1; next - vector storing transition counts for length i.
// Output: Updates prev and next vectors with the counts for the transitions of strings up to length n.
void
countNumOfAcceptHelp(int i, int n, int m, int a, mpz_class prev[][numberOfStates], mpz_class next[][numberOfStates]) {
    // Base case: the length i string has reached our max length n

    // Check DFAStates for the total number of validTransitions
    for (int j = 0; j < m + 1; ++j) {
        for (int DFAState: DFAStates) {
            for (char c: characters) {
                countNumOfAcceptTransition(n, m, j, DFAState, c, prev, next);
            }
        }
    }
    if (i == n) {
        return;
    }

    // All DFAStates have been checked for current length so copy the vectors
    // and initialize next[] with zeros
    for (int k = 0; k < m + 1; ++k) {
        for (int DFAState: DFAStates) {
            prev[k][DFAState] = next[k][DFAState];
            next[k][DFAState] = 0;
        }
    }
    // Recursive step: Increase the size of the string being tested
    countNumOfAcceptHelp(i + 1, n, m, a, prev, next);
}


// Starts the recursive call chain and reports the results.
// Precondition: n is a non-negative integer representing the length of strings to consider. result is an initialized GMP integer type (mpz_t).
// Postcondition: Computes the number of strings of length n that are accepted by the DFA and stores the result in the provided mpz_t variable.
// Input: n - the length of the strings to be considered; result - a GMP integer to store the final count of accepting strings.
// Output: Updates result to contain the number of accepting strings of length n.
void countNumOfAccept(int n, int m, mpz_class &result) {

//    std::vector<mpz_t> prev(numberOfStates); //  A vector of GMP integer types (mpz_t)
//    std::vector<mpz_t> next(numberOfStates); //  A vector of GMP integer types (mpz_t)
    mpz_class prev[m + 1][numberOfStates];
    mpz_class next[m + 1][numberOfStates];


    // Initializes all indexes to 1
    // For a string of length 0 every state is an accepting state
    for (int i = 0; i < m + 1; ++i) {
        for (int j = 0; j < numberOfStates; ++j) {
            if (i == 0) {
                prev[i][j] = 1;
            } else {
                prev[i][j] = 0;
            }

            next[i][j] = 0;
        }
    }

    // Start recursive call chain at string of length 1 since prev is
    // already populated for a string of length 0
    countNumOfAcceptHelp(1, n, m, 0, prev, next);
    result = next[m][0];
//    for (int i = 0; i < m + 1; ++i) {
//        for (int j = 0; j < 6; ++j) {
//            std::cout << prev[m][j] << " " << next[m][j] << " ";
//        }
//        std::cout << std::endl;;
//    }


}


int main() {
    generateDFAStates();
    int n, m;
    bool inputValid = false;
    mpz_class result;

    std::cout << "===============================" << std::endl;
    std::cout << "Summary of Accepting Strings" << std::endl;
    std::cout << "===============================" << std::endl;
    /*
    // Print results for different string lengths and numbers of 'a's
    for (int i = 0; i < 100; ++i) {
        std::cout << "Processing strings of size n = " << i << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
        mpz_class prev = 0;
        for (int j = 0; j < i+5; ++j) {
            countNumOfAccept(j, i, result);

            // Print results with alignment and formatting
            std::cout << "  With the number of 'a's m = " << std::setw(2) << i
                      << " | Accepting strings: " << std::setw(20) << result << std::endl;
            if (result == 0 && prev != 0) {
                i = j;
            }
            prev = result;
        }
        std::cout << "-----------------------------------------" << std::endl;
    }

    // Final result for the largest n
    std::cout << std::endl;
    std::cout << "===============================" << std::endl;
    std::cout << "Final Summary" << std::endl;
    std::cout << "===============================" << std::endl;
    std::cout << "For the largest string size n = " << std::setw(3) << 99;
    std::cout << ", the total number of accepting strings is: " << result << std::endl;
    */

   /*for (int i = 0; i < 100; ++i) {
        std::cout<<"====================================================="<<std::endl;
        for (int j = 0; j < i+3; ++j) {
            countNumOfAccept(i,j, result);
            std::cout << "For a string of size n = " << i ;
            std::cout << " with m A's = " << j ;
            std::cout << " the total number of accepting strings: ";
            gmp_printf("%Zd\n", result);
    
        }
      
    }*/

    while (!inputValid) {
        std::cout << "Enter the length of the strings n > 0 and n <= 300:  ";
        std::cin >> n;
        std::cout << "Enter the a number of a's:  ";
        std::cin >> m;

        if (n > 0 && n <= 300) {
            countNumOfAccept(n, m, result);
            inputValid = true;
        } else {
            std::cout << "Invalid input!" << std::endl;
        }
    }
    std::cout << "For a string of size n = " << n;
    std::cout << " the total number of accepting strings: ";
    std::cout << result << std::endl;

    return 0;
}
