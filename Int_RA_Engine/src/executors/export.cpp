#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT <relation_name> 
 */

bool syntacticParseEXPORT(bool isaTable)
{
    logger.log("syntacticParseEXPORT");
    if ((isaTable && tokenizedQuery.size() != 2) || (!isaTable && tokenizedQuery.size() != 3))
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORT;
    parsedQuery.isaTable = isaTable;
    if (isaTable)
        parsedQuery.exportRelationName = tokenizedQuery[1];
    else
        parsedQuery.exportRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseEXPORT(){
    logger.log("semanticParseEXPORT");
    //Table should exist
    if (parsedQuery.isaTable){
        if (tableCatalogue.isTable(parsedQuery.exportRelationName))
            return true;
    } else {
        if (matrixCatalogue.isMatrix(parsedQuery.exportRelationName))
            return true;
    }
    cout << "SEMANTIC ERROR: No such name exists" << endl;
    return false;
}

void executeEXPORT(){
    logger.log("executeEXPORT");
    if (parsedQuery.isaTable){
        Table* table = tableCatalogue.getTable(parsedQuery.exportRelationName);
        table->makePermanent();
    } else {
        blocksRead = 0;
        blocksWritten = 0; 
        Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.exportRelationName);
        matrix->makePermanent();
        cout << "Number of blocks read: "<<blocksRead<<endl;
        cout << "Number of blocks written: "<<blocksWritten<<endl;
        cout << "Number of blocks accessed: "<<blocksRead + blocksWritten<<endl;
    }
    return;
}