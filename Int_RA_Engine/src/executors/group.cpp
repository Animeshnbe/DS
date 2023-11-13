#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * <new_table> <- GROUP BY <grouping_attribute> FROM <table_name> HAVING
<aggregate(attribute)> <bin_op> <attribute_value> RETURN <aggregate_func(attribute)>
 * 
 */

pair<string,string> extractOp(string input){
    std::regex pattern(R"((SUM|MAX|MIN|COUNT|AVG)\(([^)]+)\))");
    std::smatch match;
    if (regex_search(input, match, pattern)) {
        if (match.size() == 3){
            string s1 = match[1].str(), s2 = match[2].str();
            return {s1, s2};
        }
    }
    return {"",""};
} 

bool syntacticParseGROUP(){
    logger.log("syntacticParseGROUP");
    vector<string> ops = {">","<","==",">=","<="};
    if(tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "HAVING" || tokenizedQuery[11] != "RETURN"
        || find(ops.begin(),ops.end(),tokenizedQuery[9])==ops.end()){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = GROUP;
    parsedQuery.groupResultRelationName = tokenizedQuery[0];
    parsedQuery.groupRelationName = tokenizedQuery[6];

    parsedQuery.groupColumn = tokenizedQuery[4];

    parsedQuery.aggColumn = extractOp(tokenizedQuery[8]);
    if (parsedQuery.aggColumn.first==""){
        cout<<"Syntax Error in return aggregate operation "<<tokenizedQuery[8]<<endl;
        return false;
    }
    parsedQuery.returnColumn = extractOp(tokenizedQuery[12]);
    if (parsedQuery.returnColumn.first==""){
        cout<<"Syntax Error in aggregate operation "<<tokenizedQuery[12]<<endl;
        return false;
    }    

    return true;
}

bool semanticParseGROUP(){
    logger.log("semanticParseGROUP");

    if(tableCatalogue.isTable(parsedQuery.groupResultRelationName)){
        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.groupRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.groupColumn, parsedQuery.groupRelationName)){
        cout<<"SEMANTIC ERROR: Column "<<parsedQuery.groupColumn<<" doesn't exist in relation"<<endl;
        return false;
    }
    
    if(!tableCatalogue.isColumnFromTable(parsedQuery.aggColumn.second, parsedQuery.groupRelationName)){
        cout<<"SEMANTIC ERROR: Column "<<parsedQuery.aggColumn.second<<" doesn't exist in relation"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.returnColumn.second, parsedQuery.groupRelationName)){
        cout<<"SEMANTIC ERROR: Column "<<parsedQuery.returnColumn.second<<" doesn't exist in relation"<<endl;
        return false;
    }

    return true;
}

void executeGROUP(){
    logger.log("executeGROUP");
    Table table = *tableCatalogue.getTable(parsedQuery.groupRelationName);
    int ind = find(table.columns.begin(),table.columns.end(),parsedQuery.groupColumn)-table.columns.begin();
    table.cols.push_back({ind,true});

    Table* temp = new Table(table);
    temp->sourceFileName = "";
    temp->tableName = parsedQuery.groupRelationName+"Temp";
    temp->indexedColumn = temp->columns[temp->cols[0].first];
    temp->indexed = true;
    tableCatalogue.insertTable(temp);
    table.tablesort(temp->tableName);

    // cout<<"agg "<<parsedQuery.aggColumn.first<<":"<<parsedQuery.aggColumn.second<<endl;
    // cout<<"ret "<<parsedQuery.returnColumn.first<<":"<<parsedQuery.returnColumn.second<<endl;
    Table* obj = new Table(parsedQuery.groupResultRelationName,{parsedQuery.groupColumn,parsedQuery.returnColumn.first+parsedQuery.returnColumn.second});

    obj->group(temp, ind, parsedQuery.aggColumn, tokenizedQuery[9], stoi(tokenizedQuery[10]), parsedQuery.returnColumn);
    if (obj->rowCount>0)
        tableCatalogue.insertTable(obj);
    else
        cout<<"Group By result is empty"<<endl;
    tableCatalogue.deleteTable(temp->tableName);
    return;
}