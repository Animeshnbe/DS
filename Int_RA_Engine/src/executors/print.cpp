#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */
bool syntacticParsePRINT(bool isaTable)
{
    logger.log("syntacticParsePRINT");
    if ((isaTable && tokenizedQuery.size() != 2) || (!isaTable && tokenizedQuery.size() != 3))
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINT;
    parsedQuery.isaTable = isaTable;
    if (isaTable)
        parsedQuery.printRelationName = tokenizedQuery[1];
    else
        parsedQuery.printRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParsePRINT()
{
    logger.log("semanticParsePRINT");
    if (parsedQuery.isaTable && !tableCatalogue.isTable(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    if (!parsedQuery.isaTable && !matrixCatalogue.isMatrix(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINT()
{
    logger.log("executePRINT");
    if (parsedQuery.isaTable){
        Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
        table->print();
    } else {
        blocksRead = 0;
        blocksWritten = 0;
        Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.printRelationName);
        matrix->print();
        cout << "Number of blocks read: "<<blocksRead<<endl;
        cout << "Number of blocks written: "<<blocksWritten<<endl;
        cout << "Number of blocks accessed: "<<blocksRead + blocksWritten<<endl;
    }
    return;
}
