#include <iostream>
#include <string>
#include <gmpxx.h>
#include <vector>
#include <cmath>
#include <vector>

using namespace std;
//run code with
//g++ p1.cpp -lgmp -lgmpxx
//./a.out

/*


*/


//int value for the one failing state in the dfa
static long FAIL_STATE = 1365;

/** **************************************************************************************
Function to encode a string into an integer using base 4 (1-4)
@pre:recieves a string which represents a string of our alphabet a,b,c,d
@post: returns a int that represents a int in our alphabet which is in base 4
 *****************************************************************************************/
int encodeToState(const string& s) {
    int encoded = 0;
    int base = 4;
    int len = s.length();
    
    for (int i = 0; i < len; ++i) {
        int value;
        switch (s[i]) {
            case 'a': value = 1; break;
            case 'b': value = 2; break;
            case 'c': value = 3; break;
            case 'd': value = 4; break;
            default: value = -1; 
        }
        //essentially doing 4^len * letter
        encoded += value * pow(base, len - i - 1);
    }
    return encoded;
}

/** **************************************************************************************
Function to decode an integer into a string using base 4 (1-4)
@pre:recieves a int which represents a string in base four in our alphabet
@post: returns a string which represents a in base four in our alphabet.
 *****************************************************************************************/
string decodeToString(int encoded) {
    string decoded = "";
    int base = 4;

    for (int i = 0; i < 5; ++i) { // Since we only care about the last 5 characters
        int value = (encoded - 1) % base;
        encoded = (encoded - 1) / base;

        switch (value) {
            case 0: decoded = 'a' + decoded; break;
            case 1: decoded = 'b' + decoded; break;
            case 2: decoded = 'c' + decoded; break;
            case 3: decoded = 'd' + decoded; break;
        }
    }
    
    return decoded;
}


/** **************************************************************************************
helper function that checks substring of size 6 for at least 1 occurrence of each symbol in alphabet.
@pre:seen string of characters seen so far
@post: returns true if all characters are seen or false if there are missing characters
*****************************************************************************************/
bool isValid(string seen){
    string alphabet = "abcd";
    for(auto s: alphabet){
        if(seen.find(s) == string::npos){
            return false;
        }
    }
    return true;
}

/** **************************************************************************************
transition function which takes us to a new state given a current state
@pre: recieves a long which represents our state, and a char representing a char in our
alphabet
@post: returns the next state using the current state and the given character.
*****************************************************************************************/
long transitionFunction(long currState, char input){
    
    //if we are already at fail state, keep returning it
    if(currState == FAIL_STATE){
        return FAIL_STATE;
    }
    string charsSeen = decodeToString(currState);
    
    charsSeen += input;
    //if we are at length 6, check substring for validity
    if(charsSeen.length() == 6){
        if(isValid(charsSeen)){
            //pop the front character (looking at next substring of 6)
            charsSeen.erase(charsSeen.begin());
        }
        else{
            return FAIL_STATE;
        }
    }
    return encodeToState(charsSeen);

}


/** **************************************************************************************
recursive helper. recurses up to n testing every combination by checking ever state 
transition given empty set up to strings of length n. alsu uses dynamic programming
to compute possibility effeciently by storing previously calculated values in a 2d 
array all recursive calls have acess to.
@pre: recieve a int representing length n + 1, a long representing the current state
in our dfa, and a mpz_class 2d array which we use to store our results once they
are calculated.
@post: return the number of valid string of length n or recurse from a given state to
transition(state,'a')+transition(state,'b')+transition(state,'c')+transition(state,'d')
*****************************************************************************************/
mpz_class NthRecurHelper(long n,long currState,mpz_class arr[][1367]){
    //base casses
    
    //if the state is ever the fail state return zero
    if(currState == FAIL_STATE){
        return arr[n][currState] = 0 ;
    //if the length is 1 then that means we end in a accepting state and should return 1
    }else if(n == 1){
        return arr[n][currState] = 1;

    //if the current position in our 2d array isnt -1 then return that value
    }else if(arr[n][currState] != -1){
        return arr[n][currState];

    //recursively call  testing the current state and the four other states it can go to.
    }else{
       return arr[n][currState] = NthRecurHelper(n-1,transitionFunction(currState,'a'),arr) + NthRecurHelper(n-1,transitionFunction(currState,'b'),arr) + NthRecurHelper(n-1,transitionFunction(currState,'c'),arr) + NthRecurHelper(n-1,transitionFunction(currState,'d'),arr) ;
    }
   
}

/** **************************************************************************************
main recursive set up function. sets up 2d array for dynamic programming,
then calls our recursive helper with a empty set and n.
@pre: recieves a n which represents the length n which our dfa accepts
@post: return the number of valid string of length n that are accepted with our dfa.
*****************************************************************************************/
mpz_class NthRecur(long n){

    //need a data structure that can store  our previous values we calculated recursively.
    // if we use a 2d array then we hava strings of length n
    mpz_class arr[n+2][1367];

    //set values in array to be -1 so we can compare them in the helper
    for (int i = 0; i < n+2; ++i) {
        for (int j = 0; j < 1367; ++j) {
            arr[i][j] = -1;
        }
    }
    return NthRecurHelper(n+1,encodeToState(""),arr);
}


/** **************************************************************************************
recursive helper. recurses up to n testing every combination by checking ever state 
transition given empty set up to strings of length n. alsu uses dynamic programming
to compute possibility effeciently by storing previously calculated values in a 2d 
array all recursive calls have acess to.
@pre: recieve a int representing length n + 1, a long representing the current state
in our dfa, and a mpz_class 2d array which we use to store our results once they
are calculated.
@post: return the number of valid string of length n or recurse from a given state to
transition(state,'a')+transition(state,'b')+transition(state,'c')+transition(state,'d')
*****************************************************************************************/
mpz_class NAndMthRecurHelper(long n,long currState,std::vector<std::vector<std::vector<mpz_class> > > vect3d,long m,long As){
    //base casses

    // uses 3d array, but overhead is way too bug :-;. could use the prev next like in richards? seems like the only way
    //so what we need to do is,
    //1) get my recurvisve calls to work in reverse? instead of startin at n and going to n = 1 start from n = 1 and 
    //go to n
    //

    //first case, regardless of amount of a's if we go to the fail state return zero for both memory storages.
    if(currState == FAIL_STATE){
        
        //given length, and at the state set that to 0
        return vect3d[n][As][currState] = 0;

    // checking if length is 1 meaning we have a string of the correct length for L
    }else if(n == 1){

        //Larray[n][currState] = 1;
        //check to see if the number of A's == m
        if(m == As){

            //return 1 since we have a string with correct length, and A's = M's
            return vect3d[n][As][currState] = 1;
        // otherwise we have the correct string length, but not the correct amount of a's 
        }else{
            return vect3d[n][As][currState] = 0;
        }
    
    //recursively call  testing the current state and the four other states it can go to.
    }else if(vect3d[n][As][currState] != -1){

        return vect3d[n][As][currState];

    }else{

       // find a way to maintain L and A's 
       return vect3d[n][As][currState] = 
       NAndMthRecurHelper(n-1,transitionFunction(currState,'a'),vect3d,m,As+1) +
       NAndMthRecurHelper(n-1,transitionFunction(currState,'b'),vect3d,m,As) + 
       NAndMthRecurHelper(n-1,transitionFunction(currState,'c'),vect3d,m,As) +
       NAndMthRecurHelper(n-1,transitionFunction(currState,'d'),vect3d,m,As) ;
    }
   
}

/** **************************************************************************************
main recursive set up function. sets up 2d array for dynamic programming,
then calls our recursive helper with a empty set and n.
@pre: recieves a n which represents the length n which our dfa accepts
@post: return the number of valid string of length n that are accepted with our dfa.
*****************************************************************************************/
mpz_class NAndMthRecur(long n,long m){

    //need a data structure that can store  our previous values we calculated recursively.
    // if we use a 2d array then we hava strings of length n

    //vector<vector<vector<mpz_class>>> vect3d[n+2][m+2][1367];
    std::vector<std::vector<std::vector<mpz_class> > > vect3d (n+2, std::vector<std::vector<mpz_class> > (m+2, std::vector<mpz_class>(1367,1)));

    for (int i = 0; i < vect3d.size(); i++) {
            //cout << "Elements at block "
            //<< i << ": ";
            // Displaying element at each column,
            // 0 is the starting iterator,
            // size() is the ending iterator
            for (int j = 0; j != vect3d[i].size(); j++) {
                //cout << "Elements at row "
                //<< j << ": ";
                for (int k = 0; k != vect3d[i][j].size(); k++) {
                    // use all indexes to get the values store in the vector
                    vect3d[i][j][k] = -1;
                    //cout << vect3d[i][j][k]<< ' ';
                    
                }
            //cout << endl;
        }
    }
    
    //idea to start geting the compund working. lets make a second 2d array to store the accepting states of 
    long As = 0;

    return NAndMthRecurHelper(n+1,encodeToState(""),vect3d,m,As);
}



/** **************************************************************************************
driver of our program
*****************************************************************************************/
int main() {
    //notes test fails on later cases?
    //abcdabcdabcdabcdabcdabcdabcdabcd
    //length 32 a's = 8 should return some values

   //testing up to 310 values

    //std::cout<< "result for strings length: "<<1<<" ,with "<<1<<" A's: " << NAndMthRecur(1,1)<<std::endl;

   
    /*for (int i = 0; i < 5; ++i) {
        std::cout<<"====================================================="<<std::endl;
        for (int j = 0; j < 5; ++j) {
            std::cout<< "result for strings length: "<<i<<" ,with "<<j<<" A's: " << NAndMthRecur(i,j)<<std::endl;
      
        
        }
      
    }*/
    cout<<endl;
   
    int input,input1;
    cout<<"N = ";
    cin>>input;
    while(!cin || input < 0 || input > 300){
        cin.clear();// clears the fail state of the cin stream
        cin.ignore(200, '\n');// clears keyboard buffer for any extra characters
            //prompts user for another input
        cout<<"Invalid value input"<<endl;
        cout<<"N = ";
        cin>>input;

    }

    cout<<"M = ";
    cin>>input1;

    while(!cin || input1 < 0 || input > 300){
        cin.clear();// clears the fail state of the cin stream
        cin.ignore(200, '\n');// clears keyboard buffer for any extra characters
            //prompts user for another input
        cout<<"Invalid value input"<<endl;
        cout<<"M = ";
        cin>>input1;
    }
    std::cout<< "result for strings length: "<<input<<" ,with "<<input1<<" A's: " << NAndMthRecur(input,input1)<<std::endl;
    
    return 0;
}

