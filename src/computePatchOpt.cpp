#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <ctime>

using namespace std;

static vector<string> originalLines;
static vector<string> targetLines;
static int N;
static int M;

inline string toString(int Number){
    return static_cast<ostringstream*>( &(ostringstream() << Number) )->str();
}

inline void initCosts(vector < vector<int> > &costMin,
	       vector < vector<int> > &choicesMade,
	       vector < vector<int> > &nbLinesDeleted) {

    costMin[1][0] = 10;
    choicesMade[1][0] = 3;
    for (int i = 2; i <= N; ++i) {
	costMin[i][0] = 15;
	choicesMade[i][0] = 4;
	nbLinesDeleted[i][0] = i;
    }

    for (int j = 1; j <= M; ++j) {
	costMin[0][j] = costMin[0][j-1] + 10 + targetLines[j-1].size() + 1;
	choicesMade[0][j] = 1;
    }

}

inline void computeCosts(vector < vector<int> > &costMin,
		  vector < vector<int> > &choicesMade,
		  vector < vector<int> > &nbLinesDeleted) {
    vector<int> listMinD;
    int Ca, Cs, Cd, CD, minD, indexMinD, indexCostMin;

    for (int j = 1; j <= M; ++j) {

	listMinD.clear();

	for (int i = 1; i <= N; ++i) {

            // Define coef
	    Ca = 10 + targetLines[j-1].size() + 1;
	    if (originalLines[i-1] != targetLines[j-1])
		Cs = 10 + targetLines[j-1].size() + 1;
	    else
		Cs = 0;
	    Cd = 10;
	    CD = 15;

	    // Calculate min for D-deletion
	    listMinD.push_back(costMin[i-1][j]);
	    minD = *min_element(listMinD.begin(), listMinD.end());
	    indexMinD = distance(listMinD.begin(), find(listMinD.begin(), listMinD.end(), minD)) + 1;

	    int values[] = {costMin[i][j-1] + Ca, costMin[i-1][j-1] + Cs, costMin[i-1][j] + Cd, minD + CD};
	    vector<int> list (values, values + sizeof(values)/sizeof(int));
	    costMin[i][j] = *min_element(list.begin(), list.end());

	    indexCostMin = distance(list.begin(), find(list.begin(), list.end(), costMin[i][j])) + 1;
	    if (indexCostMin == 2 && Cs == 0)
		indexCostMin = 5;

	    choicesMade[i][j] = indexCostMin;

	    if (indexCostMin == 4)
		nbLinesDeleted[i][j] = indexMinD;
	}
    }
}

inline void generatePatch(vector < vector<int> > &choicesMade,
		   vector < vector<int> > &nbLinesDeleted) {

    vector<string> patchLines;
    int i = N, j = M;
    while (i > 0 || j > 0) {
	switch (choicesMade[i][j]){
	case 0:
	    patchLines.insert(patchLines.begin(), "No choices possible\n");
	    break;
	case 1:
	    patchLines.insert(patchLines.begin(), targetLines[j-1] + '\n');
	    patchLines.insert(patchLines.begin(), "+ " + toString(i) + '\n');
	    --j;
	    break;
	case 2:
	    patchLines.insert(patchLines.begin(), targetLines[j-1] + '\n');
	    patchLines.insert(patchLines.begin(), "= " + toString(i) + '\n');
	    --i;
	    --j;
	    break;
	case 3:
	    patchLines.insert(patchLines.begin(), "d " + toString(i) + '\n');
	    --i;
	    break;
	case 4:
	    patchLines.insert(patchLines.begin(), "D " + toString(i+1 - nbLinesDeleted[i][j]) +
			      " " + toString(nbLinesDeleted[i][j]) + '\n');
	    i -= nbLinesDeleted[i][j];
	    break;
	case 5:
	    --i;
	    --j;
	    break;
	}
    }

    for (uint i = 0; i < patchLines.size(); ++i)
	cout << patchLines[i];
}

int main(int argc, char* argv[]) {

    ifstream oFile(argv[1]);
    ifstream tFile(argv[2]);

    string line;
    while (getline(oFile, line))
	originalLines.push_back(line);

    while (getline(tFile, line))
	targetLines.push_back(line);

    N = originalLines.size();
    M = targetLines.size();

    // Choices :
    // 0 -> ERREUR !
    // 1 -> add
    // 2 -> substitution
    // 3 -> deletion
    // 4 -> D-deletion
    // 5 -> identity

    vector<vector <int> > costMin(N+1,vector<int>(M+1));
    vector<vector <int> > choicesMade(N+1, vector<int>(M+1));
    vector<vector <int> > nbLinesDeleted(N+1, vector<int>(M+1));

    initCosts(costMin, choicesMade, nbLinesDeleted);
    computeCosts(costMin, choicesMade, nbLinesDeleted);
    generatePatch(choicesMade, nbLinesDeleted);
}
