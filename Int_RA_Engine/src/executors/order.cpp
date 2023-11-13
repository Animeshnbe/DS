#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * result <- ORDER BY column_name <ASC|DESC> ON relation_name
 * 
 */
bool syntacticParseORDER(){
    logger.log("syntacticParseORDER");
    parsedQuery.queryType = ORDER;
    parsedQuery.sortRelationName = tokenizedQuery[7];
    parsedQuery.sortResultRelationName = tokenizedQuery[0];
    bool sortingStrategy;
    string stype = tokenizedQuery[5];
    if (stype=="ASC")
        sortingStrategy = true;
    else if (stype=="DESC")
        sortingStrategy = false;
    else {
        cout<<"SYNTAX ERROR in SORT ORDER "<<stype<<endl;
        return false;
    }
    parsedQuery.sortColumns.push_back({tokenizedQuery[4],sortingStrategy});
    
    return true;
}

bool semanticParseORDER(){
    logger.log("semanticParseORDER");

    if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    // cout<<"col "<<col.first<<":"<<col.second<<endl;
    string col = parsedQuery.sortColumns[0].first;
    if(!tableCatalogue.isColumnFromTable(col, parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Column "<<col<<" doesn't exist in relation"<<endl;
        return false;
    }
    
    return true;
}

void executeORDER(){
    logger.log("executeORDER");
    Table table = *tableCatalogue.getTable(parsedQuery.sortRelationName);
    int ind = find(table.columns.begin(),table.columns.end(),parsedQuery.sortColumns[0].first)-table.columns.begin();
    table.cols.push_back({ind,parsedQuery.sortColumns[0].second});
    
    Table* obj = new Table(table);
    obj->sourceFileName = "";
    obj->tableName = parsedQuery.sortResultRelationName;
    obj->indexedColumn = obj->columns[obj->cols[0].first];
    obj->indexed = true;
    tableCatalogue.insertTable(obj);
    table.tablesort(parsedQuery.sortResultRelationName);
    // Table obj2 = *tableCatalogue.getTable(parsedQuery.sortResultRelationName);
    return;
}