/** @file computePatchOpt.cpp
 *  @brief Computes an optimal patch between two files F1 and F2
 *
 *
 *  @author Ben LEROUX (ISI1)
 *  @author Pierre THALAMY (ISI2)
 *
 */

#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <climits>

using namespace std;

/** \brief number of lines in source file */
static int N;
/** \brief number of lines in target file */
static int M;
/** \brief Lines from source file */
static vector<string> originalLines;
/** \brief Lines from target file */
static vector<string> targetLines;

/** @brief Represents a type of operational choice
 */
typedef enum {
    OpChoiceAdd = 0,
    OpChoiceSub = 1,
    OpChoiceID = 2,
    OpChoiceDel = 3,
    OpChoiceDDel = 4
} OpChoice;


/** @brief Convert integer into string
 *
 *  @param Number The integer to convert
 *  @return The string from the integer
 */
inline string toString(int Number){
   return static_cast<ostringstream*>( &(ostringstream() << Number) )->str();
}

/** @brief Initializes the constant values from the cost and choices matrices
 *
 *  @param costMin the cost matrix
 *  @param choicesMade the choices matrix
 *  @return Void.
 */
inline void initCosts(vector < vector<int> > &costMin,
		      vector < vector<int> > &choicesMade) {

    costMin[1][0] = 10;
    choicesMade[1][0] = OpChoiceDel;
    for (int i = 2; i <= N; ++i) {
    	costMin[i][0] = 15;
    	choicesMade[i][0] = OpChoiceDDel + i;
    }

    for (int j = 1; j <= M; ++j) {
    	costMin[0][j] = costMin[0][j-1] + 10 + targetLines[j-1].size() + 1;
    	choicesMade[0][j] = OpChoiceAdd;
    }

}

/** @brief Iteratively computes the content of the choices matrix so as to minimize
 *  the values of the cost matrix
 *
 *  @param costMin the cost matrix
 *  @param choicesMade the choices matrix
 *  @return Void.
 */
inline void computeCosts(vector < vector<int> > &costMin,
			 vector < vector<int> > &choicesMade) {
    int Ca, Cs, minD, indexMinD, newMinD, currentIndex;
    const int Cd = 10;		// Cost of a single deletion
    const int CD = 15;		// Cost of multiple deletion


    for (int j = 1; j <= M; ++j) {

	minD = INT_MAX;		// Minimum cost of multiple deletion
	indexMinD = 0;		// Index of line with min deletion from it
	currentIndex = 0;	// Current index of considered line
	Ca = 10 + targetLines[j-1].size() + 1; // Cost of adding line j from target file

	for (int i = 1; i <= N; ++i) {

	    currentIndex++;

            // Compute cost of substitution operation
	    if (originalLines[i-1] != targetLines[j-1])
		Cs = 10 + targetLines[j-1].size() + 1;
	    else
		Cs = 0;

	    // Calculate min for D-deletion
	    newMinD = min(minD, costMin[i-1][j]);
	    if (newMinD == costMin[i-1][j])
		indexMinD = currentIndex;
	    minD = newMinD;

	    // Find out what operation has minimum cost
	    int currentCosts[] = {costMin[i][j-1] + Ca, costMin[i-1][j-1] + Cs,
				  costMin[i-1][j] + Cd, minD + CD};
	    costMin[i][j] = *min_element(currentCosts, currentCosts + 4);

	    // Proceed with minimum-cost choice
	    if (costMin[i][j] == costMin[i][j-1] + Ca) // Addition
		choicesMade[i][j] = OpChoiceAdd;
	    else if (costMin[i][j] == costMin[i-1][j-1] + Cs)
		if (Cs != 0)
		    choicesMade[i][j] = OpChoiceSub; // Substitution
		else
		    choicesMade[i][j] = OpChoiceID; // Identity
	    else if (costMin[i][j] == costMin[i-1][j] + Cd)
		choicesMade[i][j] = OpChoiceDel; // Single Deletion
	    else {
		choicesMade[i][j] = OpChoiceDDel; // Multiple Deletions
		choicesMade[i][j] += indexMinD;	  // Number of deleted lines
	    }

	}
    }
}

/** @brief Generate the patch based on the content of the choices matrix
 *  and print it to stdout
 *  @param choicesMade the choices matrix
 *  @return Void.
 */
inline void generatePatch(vector < vector<int> > &choicesMade) {

    vector<string> patchLines;
    int i = N, j = M;
    while (i > 0 || j > 0) {
	switch (choicesMade[i][j]){
	case OpChoiceAdd:
	    patchLines.insert(patchLines.begin(), targetLines[j-1] + '\n');
	    patchLines.insert(patchLines.begin(), "+ " + toString(i) + '\n');
	    --j;
	    break;
	case OpChoiceSub:
	    patchLines.insert(patchLines.begin(), targetLines[j-1] + '\n');
	    patchLines.insert(patchLines.begin(), "= " + toString(i) + '\n');
	    --i;
	    --j;
	    break;
	case OpChoiceDel:
	    patchLines.insert(patchLines.begin(), "d " + toString(i) + '\n');
	    --i;
	    break;
	case OpChoiceID:
	    --i;
	    --j;
	    break;
	default: //DDelection
	    patchLines.insert(patchLines.begin(), "D " +
			      toString(i+1 - choicesMade[i][j] + OpChoiceDDel) +
			      " " + toString(choicesMade[i][j] - OpChoiceDDel) + '\n');
	    i -= (choicesMade[i][j] - OpChoiceDDel); // Decrement by number of deleted lines
	    break;
	}
    }

    // Print to stdout
    for (uint i = 0; i < patchLines.size(); ++i)
	cout << patchLines[i];
}

/** @brief The main function
 *
 *  @param No arguments
 *  @return int
 */
int main(int argc, char* argv[]) {

    // Read both files and store content in String vectors indexed by line number
    ifstream oFile(argv[1]);
    ifstream tFile(argv[2]);

    string line;
    while (getline(oFile, line))
    	originalLines.push_back(line);

    while (getline(tFile, line))
    	targetLines.push_back(line);

    N = originalLines.size();
    M = targetLines.size();

    // Initialize and compute cost and choices matrices
    vector<vector <int> > costMin(N+1,vector<int>(M+1));
    vector<vector <int> > choicesMade(N+1, vector<int>(M+1));

    initCosts(costMin, choicesMade);
    computeCosts(costMin, choicesMade);

    generatePatch(choicesMade);

    oFile.close();
    tFile.close();
}
