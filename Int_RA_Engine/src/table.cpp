#include "global.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName 
 * @param columns 
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (unsigned int)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (unsigned int)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    unsigned int count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}



/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

        if (cursor->pageIndex < this->blockCount - 1)
        {
            cursor->nextPage(cursor->pageIndex+1);
        }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    bufferManager.deleteFromPool(this->tableName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
    return -1;
}

// bool comp(vector<int>& a, vector<int>& b){
//     for (int i=0;i<this->cols.size();i++){
//         bool ord = cols[i].second;
//         int ind = cols[i].first;
//         if (a[ind]==b[ind]) continue;
//         if (ord) {
//             if (a[ind]<b[ind]) return true;
//             else if (a[ind]>b[ind]) return false;
//         } else {
//             if (a[ind]>b[ind]) return true;
//             else if (a[ind]<b[ind]) return false;
//         }
//     }
//     return true;
// }

/**
 * @brief Function that sorts the relation indicated by cols
 * 
 * @param name of the table to be assigned to 
 * @return int 
 */
void Table::tablesort(string newname){
    bool inplace = newname.empty();
    if (inplace)
        logger.log("Table::Sort inplace");
    else 
        logger.log("Table::Sort assigning to "+newname);
    
    newname = (inplace==true)?this->tableName:newname;

    auto comp = [this](const vector<int>& a, const vector<int>& b) {
        // std::cout << this->cols.size() << ":" << this->cols[0] <<std::endl;
        for (int x = 0; x < this->cols.size(); x++) {
            bool ord = this->cols[x].second;
            int ind = this->cols[x].first;
            if (a[ind] == b[ind]) continue;
            if (ord) {
                if (a[ind] < b[ind]) return true;
                else return false;
            } else {
                if (a[ind] > b[ind]) return true;
                else return false;
            }
        }
        return true;
    };

    vector<vector<int>> rows; //1 block contents
    int pi = 0, lvl = 0;

    auto comp2 = [this](const pair<int,int>& a, const pair<int,int>& b) {
        for (int i = 0; i < cols.size(); i++) {
            bool ord = cols[i].second;
            int ind = cols[i].first;
            if (spages[a.first][a.second][ind] == spages[b.first][b.second][ind]) continue;
            if (ord) {
                if (spages[a.first][a.second][ind] < spages[b.first][b.second][ind]) return false;
                else if (spages[a.first][a.second][ind] > spages[b.first][b.second][ind]) return true;
            } else {
                if (spages[a.first][a.second][ind] > spages[b.first][b.second][ind]) return false;
                else if (spages[a.first][a.second][ind] < spages[b.first][b.second][ind]) return true;
            }
        }
        return true;
    };

    priority_queue<pair<int,int>,vector<pair<int,int>>,decltype(comp2)> pq(comp2);
    
    int dm = 9; //degree of merge
    do {
        pi = 0;
        for (int i=0;i<blockCount;i+=pow(dm,(lvl+1))){
            spages.clear();
            
            for (int j=0;j<pow(dm,(lvl+1));j+=pow(dm,lvl)){
                if (i+j>=blockCount) break;     
                Page p;     
                if (lvl==0 && !inplace)
                    p = bufferManager.getPage(this->tableName, i+j);
                else
                    p = bufferManager.getPage(newname, i+j);
                rows = p.getRows();
                if (this->rowsPerBlockCount[i+j]<this->maxRowsPerBlock)
                    rows.erase(rows.begin() + this->rowsPerBlockCount[i+j], rows.end());
                
                if (lvl==0)
                    sort(rows.begin(), rows.end(), comp);
                spages[j] = rows;
                pq.push({j,0});
            }

            vector<vector<int>> wb;
            
            while (!pq.empty()){
                auto [ind, ptr] = pq.top();
                wb.push_back(spages[ind][ptr]);
                pq.pop();
                ptr++;
                if (ptr<this->rowsPerBlockCount[i+ind])
                    pq.push({ind,ptr});
                else {
                    int block = ind%int(pow(dm,lvl));
                    if (block<pow(dm,lvl)-1 && i+ind+1<blockCount){
                        spages.erase(ind);
                        ind++;
                        Page p = bufferManager.getPage(newname, i+ind);
                        vector<vector<int>> rows = p.getRows();
                        spages[ind] = rows;
                        pq.push({ind,0});
                    }
                }

                if (wb.size() == this->maxRowsPerBlock){
                    bufferManager.writePage(newname+"#"+to_string(lvl),pi++,wb,this->maxRowsPerBlock);
                    wb.clear();
                }
            }

            if (!wb.empty()){
                // cout<<"Writing "<<newname+to_string(lvl)<<" for "<<pi<<endl;
                bufferManager.writePage(newname+"#"+to_string(lvl),pi++,wb,wb.size());
            }
            
            for (int j=0;j<pow(dm,(lvl+1));j++){
                if (i+j>=blockCount) break;
                if (lvl>0 || inplace)
                    bufferManager.deleteFile(newname,i+j);
            }
        }
        for (int i=0; i < blockCount; i++) {
            string oldFilename = "../data/temp/"+newname+"#"+to_string(lvl) + "_Page" + to_string(i);
            string newFilename = "../data/temp/"+newname + "_Page" + to_string(i);
            
            // cout<<"Old "<<oldFilename<<", N "<<newFilename<<endl;
            if (rename(oldFilename.c_str(),newFilename.c_str())!=0){
                cout<<"Some error renaming page blocks";
                return;
            }
        }
        lvl++;
    } while (pow(dm,lvl)<blockCount);

    bufferManager.deleteFromPool(this->tableName);

    if (inplace)
        this->indexedColumn = columns[cols[0].first];
}

/**
 * @brief Function that joins 2 relations on a col
 * 
 * @param name of the table to be assigned to 
 * @return int 
 */
void Table::join(Table& ftable, Table& stable,string binop)
{
    Table* temp = new Table(ftable);
    temp->sourceFileName = "";
    temp->tableName = ftable.tableName+"Temp";
    tableCatalogue.insertTable(temp);
    int ind1 = find(ftable.columns.begin(),ftable.columns.end(),parsedQuery.joinFirstColumnName)-ftable.columns.begin();
    ftable.cols.push_back({ind1,true});
    ftable.tablesort(temp->tableName);

    Table* temp2 = new Table(stable);
    temp2->sourceFileName = "";
    temp2->tableName = stable.tableName+"Temp";
    tableCatalogue.insertTable(temp2);
    int ind2 = find(stable.columns.begin(),stable.columns.end(),parsedQuery.joinSecondColumnName)-stable.columns.begin();
    stable.cols.push_back({ind2,true});
    stable.tablesort(temp2->tableName);

    Cursor fcursor = temp->getCursor();
    Cursor scursor= temp2->getCursor();
    vector<vector<int>> mat;
    vector<int> row1=fcursor.getNext();
    vector<int> row2=scursor.getNext();

    while(!row1.empty() && !row2.empty())
    {
        if(binop=="==")
        {
            // cout << mat.size() << " "<<this->maxRowsPerBlock <<" "<<ind1 <<" "<<ind2<<"\n";
            // cout << row1[ind1] << " "<< row2[ind2] <<"\n";
            if(row1[ind1]>row2[ind2])
            {
                row2=scursor.getNext();
            }
            else if(row1[ind1]<row2[ind2])
            {
                row1=fcursor.getNext();
            }
            else if(row1[ind1]==row2[ind2])
            {
                vector<int> res;
                res.insert(res.end(),row1.begin(),row1.end());
                res.insert(res.end(),row2.begin(),row2.end());
                mat.push_back(res);
                this->rowCount++;
                checkwrite(mat);

                Cursor tempCursor1=Cursor(parsedQuery.joinFirstRelationName,fcursor.pageIndex);
                tempCursor1.pagePointer=fcursor.pagePointer;
                vector<int> tempRow1=tempCursor1.getNext(); 
                while(!tempRow1.empty() && tempRow1[ind1]==row2[ind2])
                {
                    res.clear();
                    res.insert(res.end(),tempRow1.begin(),tempRow1.end());
                    res.insert(res.end(),row2.begin(),row2.end());
                    mat.push_back(res);
                    this->rowCount++;
                    checkwrite(mat);
                    tempRow1=tempCursor1.getNext();
                }
                row2=scursor.getNext();
                // Cursor tempCursor2=Cursor(parsedQuery.joinSecondRelationName,scursor.pageIndex);
                // tempCursor2.pagePointer=scursor.pagePointer;
                // vector<int> tempRow2=tempCursor2.getNext(); 
                // while(row1[ind1]==tempRow2[ind2])
                // {
                //     res.clear();
                //     res.insert(res.end(),row1.begin(),row1.end());
                //     res.insert(res.end(),tempRow2.begin(),tempRow2.end());
                //     mat.push_back(res);
                //     this->rowCount++;
                //     checkwrite(mat);
                // }
                // row1=fcursor.getNext();
            }
        }
        else if(binop==">")
        {
            if(row1[ind1]<=row2[ind2])
            {
                row1=fcursor.getNext();
            }
            else if(row1[ind1]>row2[ind2])
            {
                vector<int> res;
                res.insert(res.end(),row1.begin(),row1.end());
                res.insert(res.end(),row2.begin(),row2.end());
                mat.push_back(res);
                this->rowCount++;
                checkwrite(mat);

                Cursor tempCursor1=Cursor(parsedQuery.joinFirstRelationName,fcursor.pageIndex);
                tempCursor1.pagePointer=fcursor.pagePointer;
                vector<int> tempRow1=tempCursor1.getNext(); 
                while(!tempRow1.empty() && tempRow1[ind1]>row2[ind2])
                {
                    res.clear();
                    res.insert(res.end(),tempRow1.begin(),tempRow1.end());
                    res.insert(res.end(),row2.begin(),row2.end());
                    mat.push_back(res);
                    this->rowCount++;
                    checkwrite(mat);
                    tempRow1=tempCursor1.getNext();
                }
                row2=scursor.getNext();
            }
        }
        else if(binop=="<")
        {
            if(row1[ind1]>=row2[ind2])
            {
                row2=scursor.getNext();
            }
            else if(row1[ind1]<row2[ind2])
            {
                vector<int> res;
                res.insert(res.end(),row1.begin(),row1.end());
                res.insert(res.end(),row2.begin(),row2.end());
                mat.push_back(res);
                this->rowCount++;
                checkwrite(mat);

                Cursor tempCursor2=Cursor(parsedQuery.joinSecondRelationName,scursor.pageIndex);
                tempCursor2.pagePointer=scursor.pagePointer;
                vector<int> tempRow2=tempCursor2.getNext(); 
                while(!tempRow2.empty() && row1[ind1]<tempRow2[ind2])
                {
                    res.clear();
                    res.insert(res.end(),row1.begin(),row1.end());
                    res.insert(res.end(),tempRow2.begin(),tempRow2.end());
                    mat.push_back(res);
                    this->rowCount++;
                    checkwrite(mat);
                    tempRow2=tempCursor2.getNext();
                }
                row1=fcursor.getNext();
            }
        }
        else if(binop==">=")
        {
            if(row1[ind1]<row2[ind2])
            {
                row1=fcursor.getNext();
            }
            else if(row1[ind1]>=row2[ind2])
            {
                vector<int> res;
                res.insert(res.end(),row1.begin(),row1.end());
                res.insert(res.end(),row2.begin(),row2.end());
                mat.push_back(res);
                this->rowCount++;
                checkwrite(mat);

                Cursor tempCursor1=Cursor(parsedQuery.joinFirstRelationName,fcursor.pageIndex);
                tempCursor1.pagePointer=fcursor.pagePointer;
                vector<int> tempRow1=tempCursor1.getNext(); 
                while(!tempRow1.empty() && tempRow1[ind1]>=row2[ind2])
                {
                    res.clear();
                    res.insert(res.end(),tempRow1.begin(),tempRow1.end());
                    res.insert(res.end(),row2.begin(),row2.end());
                    mat.push_back(res);
                    this->rowCount++;
                    checkwrite(mat);
                    tempRow1=tempCursor1.getNext();
                }
                row2=scursor.getNext();
            }
        }
        else if(binop=="<=")
        {
            if(row1[ind1]>row2[ind2])
            {
                row2=scursor.getNext();
            }
            else if(row1[ind1]<=row2[ind2])
            {
                vector<int> res;
                res.insert(res.end(),row1.begin(),row1.end());
                res.insert(res.end(),row2.begin(),row2.end());
                mat.push_back(res);
                this->rowCount++;
                checkwrite(mat);

                Cursor tempCursor2=Cursor(parsedQuery.joinSecondRelationName,scursor.pageIndex);
                tempCursor2.pagePointer=scursor.pagePointer;
                vector<int> tempRow2=tempCursor2.getNext(); 
                while(!tempRow2.empty() && row1[ind1]<=tempRow2[ind2])
                {
                    res.clear();
                    res.insert(res.end(),row1.begin(),row1.end());
                    res.insert(res.end(),tempRow2.begin(),tempRow2.end());
                    mat.push_back(res);
                    this->rowCount++;
                    checkwrite(mat);
                    tempRow2=tempCursor2.getNext();
                }
                row1=fcursor.getNext();
            }
        }
    }
    checkwrite(mat,true);
    if (this->rowCount == 0)
        cout<<"Join result is empty"<<endl;

    tableCatalogue.deleteTable(temp->tableName);
    tableCatalogue.deleteTable(temp2->tableName);
    // for(int i=0;i<ftable.rowCount;i++)
    // {
    //     vector<int> frow = fcursor.getNext();
    //     int startIndex=0,endIndex=stable.blockCount-1,firstOccurenceIndex=-1;
    //     while(startIndex<=endIndex)
    //     {
    //         int midIndex=startIndex+(endIndex-startIndex)/2;
    //         scursor.nextPage(midIndex);
    //         vector<int> srow=scursor.getNext();
    //         if(binop=="==")
    //         {
    //             if(frow[ind1]=srow[ind2])
    //             {
    //                 firstOccurenceIndex=midIndex;
    //                 endIndex=midIndex-1;
    //             }
    //             else if(frow[ind1]>srow[ind2])
    //             {
    //                 startIndex=midIndex+1;
    //             }
    //             else 
    //             {
    //                 endIndex=midIndex-1;
    //             }
    //         }
    //         else if(binop==">")
    //         {
    //             if(frow[ind1]>srow[ind2])
    //             {
    //                 firstOccurenceIndex=midIndex;
    //                 startIndex=midIndex+1;
    //             }
    //             else 
    //             {
    //                 endIndex=midIndex-1;
    //             }
    //         }
    //         else if(binop=="<")
    //         {
    //             if(frow[ind1]<srow[ind2])
    //             {
    //                 firstOccurenceIndex=midIndex;
    //                 endIndex=midIndex-1;
    //             }
    //             else 
    //             {
    //                 startIndex=midIndex+1;
    //             }
    //         }
    //         else if(binop==">=")
    //         {
    //             if(frow[ind1]>=srow[ind2])
    //             {
    //                 firstOccurenceIndex=midIndex;
    //                 startIndex=midIndex+1;
    //             }
    //             else 
    //             {
    //                 endIndex=midIndex-1;
    //             }
    //         }
    //         else if(binop=="<=")
    //         {
    //             if(frow[ind1]<=srow[ind2])
    //             {
    //                 firstOccurenceIndex=midIndex;
    //                 endIndex=midIndex-1;
    //             }
    //             else 
    //             {
    //                 startIndex=midIndex+1;
    //             }
    //         }
    //     }
    //     if(binop=="==")
    //     {
    //         scursor.nextPage(firstOccurenceIndex);
    //         vector<int
    //     }
    // }
}

double init(const string& op){
    if (op=="COUNT")
        return 0;
    if (op=="SUM")
        return 0;
    if (op=="AVG")
        return 0;
    if (op=="MIN")
        return INT_MAX;
    if (op=="MAX")
        return INT_MIN;
    else
        return 0;
}
double doop(string op, double cur, int next,int count){
    if (op=="SUM") cur+=next;
    else if (op=="AVG") cur = double(cur*(count-1)+next)/(count*1.0);
    else if (op=="MAX") cur = max(int(cur),next);
    else if (op=="MIN") cur = min(int(cur),next);
    else cur++;
    return cur;
}

void Table::checkwrite(vector<vector<int>>& mat, bool dowrite){
    if ((!mat.empty() && dowrite) || mat.size()==this->maxRowsPerBlock){
        // cout<<"Writing "<<this->tableName<<"_"<<this->blockCount<<endl;
        bufferManager.writePage(this->tableName, this->blockCount, mat, mat.size());
        this->blockCount++;
        this->rowsPerBlockCount.push_back(mat.size());
        mat.clear();
    }
}

/**
 * @brief Function to group table by ind column and filter 
 * and return the aggregate columns
 * 
 * @param name of the table to be assigned to 
 * @return int 
 */
void Table::group(Table* table, int ind, pair<string,string> agg, string binop, int val, pair<string,string> ret){
    logger.log("Table::group "+to_string(ind)+" "+binop+" "+to_string(val)+" "+agg.first+agg.second+"; "+ret.first+ret.second);
    int cur = INT_MIN, k=0;
    double cond=init(agg.first), res=init(ret.first);
    Cursor cursor = table->getCursor();

    int gby = table->getColumnIndex(agg.second);
    int retind = table->getColumnIndex(ret.second);

    // cout<<"Aggreg over "<<gby<<endl;
    // cout<<"Online calc over "<<retind<<endl;
    vector<vector<int>> mat;
    for (int i=0; i<table->rowCount; i++)
    {
        vector<int> row = cursor.getNext();
        if (row[ind]==cur){
            k++;
            cond = doop(agg.first,cond,row[gby],k);
            res = doop(ret.first,res,row[retind],k);
        } else {
            k = 1;

            if (cur!=INT_MIN){
                if (binop=="=="){
                    if (cond == val){
                        mat.push_back({cur,int(res)});
                        this->rowCount++;
                        checkwrite(mat);
                    }
                } else if (binop==">"){
                    if (cond > val){
                        mat.push_back({cur,int(res)});
                        this->rowCount++;
                        checkwrite(mat);
                    }
                } else if (binop==">="){
                    // cout<<"Writing row> "<<cur<<int(res)<<endl;
                    if (cond >= val){
                        mat.push_back({cur,int(res)});
                        this->rowCount++;
                        checkwrite(mat);
                    }
                } else if (binop=="<"){
                    if (cond < val){
                        mat.push_back({cur,int(res)});
                        this->rowCount++;
                        checkwrite(mat);
                    }
                } else {
                    if (cond <= val){
                        mat.push_back({cur,int(res)});
                        this->rowCount++;
                        checkwrite(mat);
                    }
                }
            }

            res = doop(ret.first,init(ret.first),row[retind],k);
            // cout<<res<<endl;
            cond = doop(agg.first,init(agg.first),row[gby],k);
            cur = row[ind];
        }
        // row = cursor.getNext();
    }
    if (cur!=INT_MIN){
        if (binop=="=="){
            if (cond == val){
                mat.push_back({cur,int(res)});
                this->rowCount++;
                checkwrite(mat);
            }
        } else if (binop==">"){
            if (cond > val){
                mat.push_back({cur,int(res)});
                this->rowCount++;
                checkwrite(mat);
            }
        } else if (binop==">="){
            // cout<<"Writing row> "<<cur<<int(res)<<endl;
            if (cond >= val){
                mat.push_back({cur,int(res)});
                this->rowCount++;
                checkwrite(mat);
            }
        } else if (binop=="<"){
            if (cond < val){
                mat.push_back({cur,int(res)});
                this->rowCount++;
                checkwrite(mat);
            }
        } else {
            if (cond <= val){
                mat.push_back({cur,int(res)});
                this->rowCount++;
                checkwrite(mat);
            }
        }
    }
    checkwrite(mat,true);
    // update return result
}




