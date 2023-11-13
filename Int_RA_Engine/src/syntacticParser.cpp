#include "global.h"

bool syntacticParse()
{
    logger.log("syntacticParse");
    string possibleQueryType = tokenizedQuery[0];

    if (tokenizedQuery.size() < 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    if (possibleQueryType == "CLEAR")
        return syntacticParseCLEAR();
    else if (possibleQueryType == "INDEX")
        return syntacticParseINDEX();
    else if (possibleQueryType == "LIST")
        return syntacticParseLIST();
    else if(possibleQueryType == "TRANSPOSE")
        return syntacticParseTMATRIX(false);
    else if(possibleQueryType == "COMPUTE")
        return syntacticParseCompMATRIX(false);
    else if(possibleQueryType == "CHECKSYMMETRY")
        return syntacticParseSymMATRIX(false);
    else if(possibleQueryType == "LOAD" && tokenizedQuery[1] == "MATRIX")
        return syntacticParseLOAD(false);
    else if (possibleQueryType == "LOAD")
        return syntacticParseLOAD(true);
    else if(possibleQueryType == "PRINT" && tokenizedQuery[1] == "MATRIX")
        return syntacticParsePRINT(false);
    else if (possibleQueryType == "PRINT")
        return syntacticParsePRINT(true);
    else if (possibleQueryType == "RENAME" && tokenizedQuery[1] == "MATRIX")
        return syntacticParseRENAME(false);
    else if (possibleQueryType == "RENAME")
        return syntacticParseRENAME(true);
    else if(possibleQueryType == "EXPORT" && tokenizedQuery[1] == "MATRIX")
        return syntacticParseEXPORT(false);
    else if(possibleQueryType == "EXPORT")
        return syntacticParseEXPORT(true);
    else if(possibleQueryType == "SOURCE")
        return syntacticParseSOURCE();
    else if (possibleQueryType == "SORT")
        return syntacticParseSORT();
    
    else
    {
        string resultantRelationName = possibleQueryType;
        if (tokenizedQuery[1] != "<-" || tokenizedQuery.size() < 3)
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        possibleQueryType = tokenizedQuery[2];
        if (possibleQueryType == "PROJECT")
            return syntacticParsePROJECTION();
        else if (possibleQueryType == "SELECT")
            return syntacticParseSELECTION();
        else if (possibleQueryType == "JOIN")
            return syntacticParseJOIN();
        else if (possibleQueryType == "CROSS")
            return syntacticParseCROSS();
        else if (possibleQueryType == "DISTINCT")
            return syntacticParseDISTINCT();
        // else if (possibleQueryType == "SORT")
        //     return syntacticParseSORT();
        else if (possibleQueryType == "ORDER" && tokenizedQuery.size()==8 && tokenizedQuery[3] == "BY" && tokenizedQuery[6] == "ON")
            return syntacticParseORDER();
        else if (possibleQueryType == "GROUP" && tokenizedQuery.size()==13 && tokenizedQuery[3] == "BY")
            return syntacticParseGROUP();
        else
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
    return false;
}

ParsedQuery::ParsedQuery()
{
}

void ParsedQuery::clear()
{
    logger.log("ParseQuery::clear");
    this->queryType = UNDETERMINED;

    this->clearRelationName = "";

    this->crossResultRelationName = "";
    this->crossFirstRelationName = "";
    this->crossSecondRelationName = "";

    this->distinctResultRelationName = "";
    this->distinctRelationName = "";

    this->exportRelationName = "";

    this->indexingStrategy = NOTHING;
    this->indexColumnName = "";
    this->indexRelationName = "";

    this->joinBinaryOperator = NO_BINOP_CLAUSE;
    this->joinResultRelationName = "";
    this->joinFirstRelationName = "";
    this->joinSecondRelationName = "";
    this->joinFirstColumnName = "";
    this->joinSecondColumnName = "";

    this->loadRelationName = "";

    this->printRelationName = "";

    this->projectionResultRelationName = "";
    this->projectionColumnList.clear();
    this->projectionRelationName = "";

    this->renameFromColumnName = "";
    this->renameToColumnName = "";
    this->renameRelationName = "";

    this->selectType = NO_SELECT_CLAUSE;
    this->selectionBinaryOperator = NO_BINOP_CLAUSE;
    this->selectionResultRelationName = "";
    this->selectionRelationName = "";
    this->selectionFirstColumnName = "";
    this->selectionSecondColumnName = "";
    this->selectionIntLiteral = 0;

    // this->sortingStrategy = NO_SORT_CLAUSE;
    // this->sortResultRelationName = "";
    this->sortColumns.clear();
    this->sortRelationName = "";

    this->sourceFileName = "";
}

/**
 * @brief Checks to see if source file exists. Called when LOAD command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isFileExists(string tableName)
{
    string fileName = "../data/" + tableName + ".csv";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

/**
 * @brief Checks to see if source file exists. Called when SOURCE command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isQueryFile(string fileName){
    fileName = "../data/" + fileName + ".ra";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}
