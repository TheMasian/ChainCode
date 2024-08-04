#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

using namespace std;

//point object class
class point {

    public:    
        int row, col;

        point() {
            row = 0;
            col = 0;
        };

        point(int i, int j) {
            row = i;
            col = j;
        };
};

class chainCode {
    
    public:
    //number of rows/columns, minimum/maximum values, label depicts the image value
    //lastQ is last zero, and chain direction
        int numRows, numCols, minVal, maxVal, label, lastQ, chainDir;
        vector<vector<int>> ZFVector, reConstructVector;
        //creating a 3x3 template around an undefined point
        //the positions are defined by their place in the array
        point coordOffset[8] = {point(0,1), point(-1,1), point(-1,0), point(-1,-1), 
                                point(0,-1), point(1,-1), point(1,0), point(1,1)};
        //zeroTable determines the direction of the current zero based on
        //where the last zero was from the previous point
        int zeroTable[8] = {6,0,0,2,2,4,4,6};
        point startP, currentP, nextP;

        chainCode(char* argv, istream &data, ostream &output, ostream &debug) {
            //read the first four integers in from input and set the values accordingly
            data >> numRows >> numCols >> minVal >> maxVal;
            //resize the 2D vector to be 2 rows and 2 col larger then number of rows and cols 
            //to allow iteration around each point if necessary without cutting anything 
            //of the image off(ZFVector stands for zero frame array but used vectors to create)
            ZFVector.resize(numRows + 2, vector<int>(numCols + 2, 0));
            //resize vector and initialize all values to 0
            reConstructVector.resize(numRows + 2, vector<int>(numCols + 2, 0));
            //create file for chain code inputs to be stored
            string file = (string)(argv);
            string chainCodeFileName = file.substr(0, file.find(".")) + "_chainCode.txt";
            ofstream chainCodeFile(chainCodeFileName);
            //create file for boundary to be stored using 1's and 0's
            string file2 = (string)(argv);
            string boundaryFileName = file.substr(0, file.find(".")) + "_boundary.txt";
            ofstream boundaryFile(boundaryFileName);

            loadImage(data);
            output << "After loadImage(), ZFVector as below:\n";
            prettyPrint(ZFVector, output);

            getChainCode(ZFVector, chainCodeFile, debug);
            output << "After getChainCode(), ZFVector as below:\n";
            prettyPrint(ZFVector, output);
            chainCodeFile.close();

            ifstream chainCodeFileRead(chainCodeFileName);
            constructBoundary(reConstructVector, chainCodeFileRead);
            output << "After constructBoundary(), ZFVector as below:\n";
            prettyPrint(reConstructVector, output);
            imgReformat(reConstructVector, boundaryFile);

            chainCodeFileRead.close();
            boundaryFile.close();

        };
        //loads the data from the input file into the ZFVector placed
        //within the zero frame boundaries
        void loadImage(istream &file) {
            for(int i = 1; i < numRows + 1; i++) {
                for(int j = 1; j < numCols + 1; j++) {
                    file >> ZFVector[i][j];
                };
            };            
        };
        //converts all 0's in '. ' for easier viewing
        void prettyPrint(vector<vector<int>> &ZVector, ostream &file) {

            for(int i = 1; i < numRows + 1; i++) {
                for(int j = 1; j < numCols + 1; j++) {
                    if(ZVector[i][j] == 0) {
                        file << ". ";
                    } else {
                        file << ZVector[i][j] << " ";
                    };
                };
                file << endl;
            };
            file << endl;
        };
        //reformats the image to create the same amount of space between every column
        void imgReformat(vector<vector<int>> &inVector, ostream &file) {

            file << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;

            string str = to_string(maxVal);
            int width = str.length();
            int r = 1;

            while(r < (numRows + 1)) {
                int c = 1;

                while(c < (numCols + 1)) {
                    file << inVector[r][c];
                    str = to_string(inVector[r][c]);
                    int WW = str.length();

                    while(WW <= width) {
                        file << " ";
                        WW++;
                    }
                    c++;
                }
                file << endl;
                r++;
            }
        };

        void getChainCode(vector<vector<int>> &ZVector, ostream &file, ostream &debug) {

            debug << "***Entering getChainCode Method***\n";
            file << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
            //iterates through binary image until first non-zero value is found
            //starting one row and col inward to account for zero frame
            label = 0;
            for(int i = 1; i < numRows + 1; i++) {
                for(int j = 1; j < numCols + 1; j++) {
                    if(ZFVector[i][j] > 0 && label == 0) {
                        label = ZFVector[i][j];
                        file << label << " " << i << " " << j << " ";
                        startP.row = i;
                        startP.col = j;
                        currentP.row = i;
                        currentP.col = j;
                        //lastQ is last zero found is always in the 4 position
                        //since reading the image is done left to right
                        lastQ = 4;
                    };
                };
            };
            //iterates from starting point counter clockwise until next point
            //equals the starting point
            while(!(nextP.row == startP.row && nextP.col == startP.col)) {

                lastQ = ++lastQ % 8;
                chainDir = findNextP(debug);
                file << chainDir << " ";
                nextP.row = currentP.row + coordOffset[chainDir].row;
                nextP.col = currentP.col + coordOffset[chainDir].col;
                //increase label value to mark a point has already been visited
                ZFVector[nextP.row][nextP.col] = label + 4;
                //ternary operator reflects where lastQ would be in position
                //to new point moving from old point
                lastQ = chainDir == 0 ? zeroTable[7] : zeroTable[chainDir - 1]; 
                debug << "lastQ: " << lastQ 
                    << "; currentP.row: " << currentP.row 
                    << "; currentP.col: " << currentP.col 
                    << "; nextP.row: " << nextP.row
                    << "; nextP.col: " << nextP.col << endl;
                currentP = nextP;
            } 
            debug << "***Leaving getChainCode Method***\n";
        };
        //finds the next point in the image that has a value greater than 0 based around
        //the current point
        int findNextP(ostream &debug) {

            debug << "***Entering findNextP Method***\n";

            int index = lastQ;
            bool found = false;
            //iterates through each of the lastQ possible positions until a new point is found
            while(!found) {
                int iRow = currentP.row + coordOffset[index].row;
                int jCol = currentP.col + coordOffset[index].col;
                if(ZFVector[iRow][jCol] == label || ZFVector[iRow][jCol] == (label + 4)) {
                    chainDir = index;
                    found = true;
                } else {
                    index = (index + 1) % 8;
                };
            };

            debug << "***Leaving findNextP Method***\n";
            debug << "chainDir: " << chainDir << endl;

            return chainDir;
        };
        //constructs image with only boundary outline
        void constructBoundary(vector<vector<int>> &RVector, istream &file) {

            file >> numRows >> numCols >> minVal >> maxVal;
            file >> label >> startP.row >> startP.col;

            int index;
            currentP = startP;

            while(!file.eof()) {

                file >> index;
                nextP.row = currentP.row + coordOffset[index].row;
                nextP.col = currentP.col + coordOffset[index].col;
                RVector[nextP.row][nextP.col] = label;
                currentP = nextP;
                if(nextP.row == startP.row && nextP.col == startP.col) {
                    break;
                }

            };         
        };
};

int main(int argc, char** argv) {
    //open 3 files. first for incoming data, then an output, and finally a debug file
    ifstream data(argv[1]);
    ofstream output(argv[2]);
    ofstream debug(argv[3]);
    //create object CC and pass arguments and files to constructor
    chainCode CC(argv[1], data, output, debug);
    data.close();
    output.close();
    debug.close();

    return 0;
};