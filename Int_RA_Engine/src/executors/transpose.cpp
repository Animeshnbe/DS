#include "global.h"

bool syntacticParseTMATRIX(bool isaTable)
{
    logger.log("syntacticParseTMATRIX");
    if (!isaTable && tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.tansposeMatrix = tokenizedQuery[2];
    parsedQuery.queryType = TRANSPOSE;
    return true;
}

bool semanticParseTMATRIX()
{
    logger.log("semanticParseTMATRIX");
    if (!matrixCatalogue.isMatrix(parsedQuery.tansposeMatrix))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeTMATRIX()
{
    blocksRead = 0;
    blocksWritten = 0;
    logger.log("executeTMATRIX");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.tansposeMatrix);
    matrix->transpose();
    cout << "Number of blocks read: "<<blocksRead<<endl;
    cout << "Number of blocks written: "<<blocksWritten<<endl;
    cout << "Number of blocks accessed: "<<blocksRead + blocksWritten<<endl;
}