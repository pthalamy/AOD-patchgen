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
    OpChoiceDel = 2,
    OpChoiceDDel = 3,
    OpChoiceID = 4
} OpChoice;


/** @brief Convert integer into string
 *
 *  @param Number The integer to convert
 *  @return The string from the integer
 */
inline string toString(int Number){
   return static_cast<ostringstream*>( &(ostringstream() << Number) )->str();
}

/** @brief Initializes the cost, choice and number of deleted lines matrices
 *  to the
 *
 *  @param s The string to be printed.
 *  @param len The length of the string s.
 *  @return Void.
 */
inline void initCosts(vector < vector<int> > &costMin,
		      vector < vector<int> > &choicesMade,
		      vector < vector<int> > &nbLinesDeleted) {

    costMin[1][0] = 10;
    choicesMade[1][0] = OpChoiceDel;
    for (int i = 2; i <= N; ++i) {
    	costMin[i][0] = 15;
    	choicesMade[i][0] = OpChoiceDDel;
    	nbLinesDeleted[i][0] = i;
    }

    for (int j = 1; j <= M; ++j) {
    	costMin[0][j] = costMin[0][j-1] + 10 + targetLines[j-1].size() + 1;
    	choicesMade[0][j] = OpChoiceAdd;
    }

}

/** @brief Computes the remaining of the content of the cost, choice
 *  and number of deleted lines
 *
 *  @param costMin the cost matrix
 *  @param choicesMade the choice matrix
 *  @param nbLinesDeleted a matrix that indicates the number of lines to delete to go from line i to j
 *  @return Void.
 */
inline void computeCosts(vector < vector<int> > &costMin,
			 vector < vector<int> > &choicesMade,
			 vector < vector<int> > &nbLinesDeleted) {
    // vector<int> listMinD;
    int Ca, Cs, Cd, CD, minD, indexCostMin, indexMinD, newMinD, currentIndex;
    int *costMin_ptr;
    
    
    for (int j = 1; j <= M; ++j) {

	minD = INT_MAX;
	indexMinD = 0;
	currentIndex = 0;
	
	for (int i = 1; i <= N; ++i) {
	    
	    currentIndex++;

            // Define coef
	    Ca = 10 + targetLines[j-1].size() + 1;
	    if (originalLines[i-1] != targetLines[j-1])
		Cs = 10 + targetLines[j-1].size() + 1;
	    else
		Cs = 0;
	    Cd = 10;
	    CD = 15;

	    // Calculate min for D-deletion
	    newMinD = min(minD, costMin[i-1][j]);
	    if (newMinD == costMin[i-1][j])
		indexMinD = currentIndex;
	    minD = newMinD;



	    int currentCosts[] = {costMin[i][j-1] + Ca, costMin[i-1][j-1] + Cs,
				  costMin[i-1][j] + Cd, minD + CD};
	    costMin_ptr = min_element(currentCosts, currentCosts + 4);
	    costMin[i][j] = *costMin_ptr;
	    // indexCostMin = (costMin_ptr - currentCosts) / sizeof(int);
	    // LAST LINE SHOULD BE USABLE INSTEAD OF CONDITONNAL CHECKS. BUT DOESNT WORK. WHY?

	    if (costMin[i][j] == costMin[i][j-1] + Ca)
		indexCostMin = OpChoiceAdd;
	    else if (costMin[i][j] == costMin[i-1][j-1] + Cs)
		if (Cs != 0)
		    indexCostMin = OpChoiceSub;
		else
		    indexCostMin = OpChoiceID;
	    else if (costMin[i][j] == costMin[i-1][j] + Cd)
		indexCostMin = OpChoiceDel;
	    else
		indexCostMin = OpChoiceDDel;


	    choicesMade[i][j] = indexCostMin;

	    if (indexCostMin == OpChoiceDDel)
		nbLinesDeleted[i][j] = indexMinD;
	}
    }
}

/** @brief Generate the patch based on the content of the choice and number of lines deleted matrices
 *  and print it to stdout
 *  @param choicesMade the choice matrix
 *  @param nbLinesDeleted a matrix that indicates the number of lines to delete to go from line i to j
 *  @return Void.
 */
inline void generatePatch(vector < vector<int> > &choicesMade,
			  vector < vector<int> > &nbLinesDeleted) {

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
	case OpChoiceDDel:
	    patchLines.insert(patchLines.begin(), "D " + toString(i+1 - nbLinesDeleted[i][j]) +
			      " " + toString(nbLinesDeleted[i][j]) + '\n');
	    i -= nbLinesDeleted[i][j];
	    break;
	case OpChoiceID:
	    --i;
	    --j;
	    break;
	default:
	    cerr << "ENUM ERROR!" << endl;
	    exit(EXIT_FAILURE);
	    break;
	}
    }

    // Print to stdout
    for (uint i = 0; i < patchLines.size(); ++i)
	cout << patchLines[i];
}

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

    vector<vector <int> > costMin(N+1,vector<int>(M+1));
    vector<vector <int> > choicesMade(N+1, vector<int>(M+1));
    vector<vector <int> > nbLinesDeleted(N+1, vector<int>(M+1));

    initCosts(costMin, choicesMade, nbLinesDeleted);
    costMin[1][0] = 10;
    choicesMade[1][0] = OpChoiceDel;

    computeCosts(costMin, choicesMade, nbLinesDeleted);
    generatePatch(choicesMade, nbLinesDeleted);

    ofstream result("costPatch");
    result << "Coût : " << costMin[N][M] << "\n"; 
    
    oFile.close();
    tFile.close();
}
