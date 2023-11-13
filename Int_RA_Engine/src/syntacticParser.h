#include "tableCatalogue.h"
#include "matrixCatalogue.h"

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    GROUP,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    LMATRIX,
    TRANSPOSE,
    CHECKSYMMETRY,
    COMPUTE,
    ORDER,
    PRINT,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    SOURCE,
    UNDETERMINED
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;
    bool isaTable = true;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";
    string loadMatrixName = "";
    string tansposeMatrix = "";
    string computeMatrix = "";
    string checkSymMatrix = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    // SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    vector<pair<string,bool>> sortColumns;
    string sortRelationName = "";

    string sourceFileName = "";

    string groupResultRelationName = "";
    string groupColumn; // op, column
    pair<string,string> aggColumn;
    pair<string,string> returnColumn;
    string groupRelationName = "";
    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT(bool isaTable);
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD(bool isaTable);
bool syntacticParsePRINT(bool isaTable);
bool syntacticParsePROJECTION();
bool syntacticParseRENAME(bool isaTable);
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseORDER();
bool syntacticParseGROUP();
bool syntacticParseSOURCE();
bool syntacticParseTMATRIX(bool isTable);
bool syntacticParseSymMATRIX(bool isTable);
bool syntacticParseCompMATRIX(bool isTable);
bool isFileExists(string tableName);
bool isQueryFile(string fileName);
