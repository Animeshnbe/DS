#include "global.h"

bool syntacticParseSymMATRIX(bool isaTable)
{
    logger.log("syntacticParseSymMATRIX");
     if (!isaTable && tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.checkSymMatrix = tokenizedQuery[1];
    parsedQuery.queryType = CHECKSYMMETRY;
    return true;
}

bool semanticParseSymMATRIX()
{
    logger.log("semanticParseSymMATRIX");
    if (!matrixCatalogue.isMatrix(parsedQuery.checkSymMatrix))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeSymMATRIX()
{
    blocksRead = 0;
    blocksWritten = 0;
    logger.log("executeSymMATRIX");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.checkSymMatrix);
    string out = matrix->checksymmetry()?"TRUE":"FALSE";
    cout<<out<<endl;
    cout << "Number of blocks read: "<<blocksRead<<endl;
    cout << "Number of blocks written: "<<blocksWritten<<endl;
    cout << "Number of blocks accessed: "<<blocksRead + blocksWritten<<endl;
}