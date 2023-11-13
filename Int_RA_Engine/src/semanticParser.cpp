#include"global.h"

bool semanticParse(){
    logger.log("semanticParse");
    switch(parsedQuery.queryType){
        case CLEAR: return semanticParseCLEAR();
        case CROSS: return semanticParseCROSS();
        case DISTINCT: return semanticParseDISTINCT();
        case EXPORT: return semanticParseEXPORT();
        case INDEX: return semanticParseINDEX();
        case JOIN: return semanticParseJOIN();
        case LIST: return semanticParseLIST();
        case LOAD: return semanticParseLOAD();
        case LMATRIX: return semanticParseLMATRIX();
        case TRANSPOSE: return semanticParseTMATRIX();
        case CHECKSYMMETRY: return semanticParseSymMATRIX();
        case COMPUTE: return semanticParseCompMATRIX();
        case PRINT: return semanticParsePRINT();
        case PROJECTION: return semanticParsePROJECTION();
        case RENAME: return semanticParseRENAME();
        case SELECTION: return semanticParseSELECTION();
        case SORT: return semanticParseSORT();
        case SOURCE: return semanticParseSOURCE();
        case ORDER: return semanticParseORDER();
        case GROUP: return semanticParseGROUP();
        default: cout<<"SEMANTIC ERROR"<<endl;
    }

    return false;
}