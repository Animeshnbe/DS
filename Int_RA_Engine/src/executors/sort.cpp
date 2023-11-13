#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * SORT relation_name BY <column_name1, column_name2,..., column_namek> IN
<ASC|DESC, ASC|DESC,..., ASC|DESC>
 * 
 */
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
    vector<string>::iterator f=find(tokenizedQuery.begin(), tokenizedQuery.end(), "IN");
    int l = f-tokenizedQuery.begin();
    // cout<<"Got l = "<<l<<endl;
    if(tokenizedQuery.size() < 6 || tokenizedQuery[2] != "BY" || f==tokenizedQuery.end()
        || tokenizedQuery.size()-l-1 != l-3){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = SORT;
    // parsedQuery.sortResultRelationName = tokenizedQuery[0];
    parsedQuery.sortRelationName = tokenizedQuery[1];
    bool sortingStrategy;
    for (int i=0; i<l-3; i++){
        string stype = tokenizedQuery[l+i+1];

        if (stype=="ASC")
            sortingStrategy = true;
        else if (stype=="DESC")
            sortingStrategy = false;
        else {
            cout<<"SYNTAX ERROR in SORT ORDER "<<stype<<endl;
            return false;
        }
        parsedQuery.sortColumns.push_back({tokenizedQuery[i+3],sortingStrategy});
    }
    // if(sortingStrategy == "ASC")
    //     parsedQuery.sortingStrategy = ASC;
    // else if(sortingStrategy == "DESC")
    //     parsedQuery.sortingStrategy = DESC;
    // else{
    //     cout<<"SYNTAX ERROR"<<endl;
    //     return false;
    // }
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    // if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
    //     cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
    //     return false;
    // }

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    for (auto& col:parsedQuery.sortColumns){
        // cout<<"col "<<col.first<<":"<<col.second<<endl;
        if(!tableCatalogue.isColumnFromTable(col.first, parsedQuery.sortRelationName)){
            cout<<"SEMANTIC ERROR: Column "<<col.first<<" doesn't exist in relation"<<endl;
            return false;
        }
    }

    return true;
}

void executeSORT(){
    logger.log("executeSORT");
    Table table = *tableCatalogue.getTable(parsedQuery.sortRelationName);

    int ind, vis[table.columnCount] = {0};
    for (auto& colname:parsedQuery.sortColumns){
        ind = find(table.columns.begin(),table.columns.end(),colname.first)-table.columns.begin();
        if (!vis[ind]){
            table.cols.push_back({ind,colname.second});
            vis[ind] = true;
        }
    }
    table.tablesort("");
    return;
}