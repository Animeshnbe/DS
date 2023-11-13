#include "global.h"
/**
 * @brief 
 * SYNTAX: RENAME column_name TO column_name FROM relation_name
 */
bool syntacticParseRENAME(bool isaTable)
{
    logger.log("syntacticParseRENAME");
    if (isaTable){
        if (tokenizedQuery.size() != 6 || tokenizedQuery[2] != "TO" || tokenizedQuery[4] != "FROM")
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        
        parsedQuery.renameFromColumnName = tokenizedQuery[1];
        parsedQuery.renameToColumnName = tokenizedQuery[3];
        parsedQuery.renameRelationName = tokenizedQuery[5];
    }
    else {
        if (tokenizedQuery.size() != 4)
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        parsedQuery.renameFromColumnName = tokenizedQuery[2];
        parsedQuery.renameToColumnName = tokenizedQuery[3];
    }
    parsedQuery.isaTable = isaTable;
    parsedQuery.queryType = RENAME;
    return true;
}

bool semanticParseRENAME()
{
    logger.log("semanticParseRENAME");
    if (parsedQuery.isaTable)
    {
        if (!tableCatalogue.isTable(parsedQuery.renameRelationName))
        {
            cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
            return false;
        }

        if (!tableCatalogue.isColumnFromTable(parsedQuery.renameFromColumnName, parsedQuery.renameRelationName))
        {
            cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
            return false;
        }

        if (tableCatalogue.isColumnFromTable(parsedQuery.renameToColumnName, parsedQuery.renameRelationName))
        {
            cout << "SEMANTIC ERROR: Column with name already exists" << endl;
            return false;
        }
    } else 
    {
        if (!matrixCatalogue.isMatrix(parsedQuery.renameFromColumnName))
        {
            cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
            return false;
        }
        if (matrixCatalogue.isMatrix(parsedQuery.renameToColumnName))
        {
            cout << "SEMANTIC ERROR: Matrix with the same target name already exists" << endl;
            return false;
        }
    }
    return true;
}

void executeRENAME()
{
    logger.log("executeRENAME");
    if (parsedQuery.isaTable)
    {
        Table* table = tableCatalogue.getTable(parsedQuery.renameRelationName);
        table->renameColumn(parsedQuery.renameFromColumnName, parsedQuery.renameToColumnName);
    } else
    {
        // cout<<"Hello world"<<endl;
        Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.renameFromColumnName);
        if (!matrix->renameMatrix(parsedQuery.renameToColumnName))
            cout<<"\nERROR: Could not rename matrix successfully"<<endl;
        matrixCatalogue.print();
    }
    return;
}