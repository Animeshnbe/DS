#include "global.h"

bool syntacticParseCompMATRIX(bool isaTable)
{
    logger.log("syntacticParseCompMATRIX");
    if (!isaTable && tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.computeMatrix = tokenizedQuery[1];
    parsedQuery.queryType = COMPUTE;
    return true;
}

bool semanticParseCompMATRIX()
{
    logger.log("semanticParseCompMATRIX");
    if (!matrixCatalogue.isMatrix(parsedQuery.computeMatrix))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    if(matrixCatalogue.isMatrix(parsedQuery.computeMatrix+"_RESULT"))
    {
        cout << "SEMANTIC ERROR: " + parsedQuery.computeMatrix + "_RESULT Matrix already exists.";
        return false;
    }
    return true;
}

void executeCompMATRIX()
{
    blocksRead = 0;
    blocksWritten = 0;
    logger.log("executeCompMATRIX");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.computeMatrix);
    matrix->compute();
    cout << "Number of blocks read: "<<blocksRead<<endl;
    cout << "Number of blocks written: "<<blocksWritten<<endl;
    cout << "Number of blocks accessed: "<<blocksRead + blocksWritten<<endl;
}