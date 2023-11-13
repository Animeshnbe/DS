#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD relation_name
 */
bool syntacticParseLOAD(bool isaTable)
{
    logger.log("syntacticParseLOAD");
    if ((isaTable && tokenizedQuery.size() != 2) || 
    (!isaTable && tokenizedQuery.size() != 3))
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if (isaTable){
        parsedQuery.loadRelationName = tokenizedQuery[1];
        parsedQuery.queryType = LOAD;
    }
    else{
        parsedQuery.loadMatrixName = tokenizedQuery[2];
        parsedQuery.queryType = LMATRIX;
    }
    return true;
}

bool semanticParseLOAD()
{
    logger.log("semanticParseLOAD");
    if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

bool semanticParseLMATRIX()
{
    logger.log("semanticParseLOAD");
    if (matrixCatalogue.isMatrix(parsedQuery.loadMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadMatrixName)) {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOAD()
{
    logger.log("executeLOAD");

    Table *table = new Table(parsedQuery.loadRelationName);
    if (table->load())
    {
        tableCatalogue.insertTable(table);
        cout << "Loaded Table. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
    }
    return;
}

void executeLMATRIX()
{
    logger.log("executeLMATRIX");
    // cout<<"Hello world"<<endl;
    Matrix *matrix = new Matrix(parsedQuery.loadMatrixName);
    blocksRead = 0;
    blocksWritten = 0;
    if (matrix->load())
    {
        matrixCatalogue.insertMatrix(matrix);
        cout << "Loaded Matrix. Column Count: " << matrix->rowCount << "; # of pages: " << matrix->blockCount << endl;
        cout << "Number of blocks read: "<<blocksRead<<endl;
        cout << "Number of blocks written: "<<blocksWritten<<endl;
        cout << "Number of blocks accessed: "<<blocksRead + blocksWritten<<endl;
    } else 
        cout<< "Load matrix error"<<endl;
    
    return;
}