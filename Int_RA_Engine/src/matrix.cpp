#include "global.h"

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Matrix::Matrix(string matrixName){
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
    this->maxRowsPerBlock = (uint)(sqrt((BLOCK_SIZE * 1024) / sizeof(int)));
}

/**
 * @brief Construct a new Matrix:: Matrix object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param matrixName 
 * @param columns 
 */
Matrix::Matrix(string matrixName, int n){
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/temp/" + matrixName + ".csv";
    this->matrixName = matrixName;
    this->rowCount = n;
    this->maxRowsPerBlock = (uint)(sqrt((BLOCK_SIZE * 1024) / sizeof(int)));
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if(getline(fin, line))
    {
        fin.close();
        if (!this->rowCount)
            this->rowCount = count(line.begin(),line.end(),',')+1;
        
        return this->blockify();
    }
    fin.close();
    return false;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<vector<int>> matrixsInPages(ceil((double)this->rowCount/this->maxRowsPerBlock));
    int rowCounter=0,columnCounter=0;
    int matrixCounter=0;
    while(getline(fin,line))
    {
        stringstream s(line);
        while(getline(s,word,','))
        {
            // cout << rowCounter << " "<< columnCounter <<" "<< matrixCounter<<" "<<this->rowCount<<"\n";
            matrixsInPages[matrixCounter].push_back(stoi(word));
            if(rowCounter==this->rowCount-1)
            {
                int rowSize=(rowCounter%this->maxRowsPerBlock)+1;
                if(columnCounter%this->maxRowsPerBlock==this->maxRowsPerBlock-1)
                {
                    bufferManager.writePage(this->matrixName,this->blockCount,matrixsInPages[matrixCounter],rowSize,min(rowCount,this->maxRowsPerBlock));
                    this->blockCount++;
                    this->perBlockCount.emplace_back(rowSize,min(rowCount,this->maxRowsPerBlock));
                    matrixsInPages[matrixCounter].clear();
                }
                else if(columnCounter==this->rowCount-1)
                {
                    int colSize=(columnCounter%this->maxRowsPerBlock)+1;
                    bufferManager.writePage(this->matrixName,this->blockCount,matrixsInPages[matrixCounter],rowSize,colSize);
                    this->blockCount++;
                    this->perBlockCount.emplace_back(rowSize,colSize);
                    matrixsInPages[matrixCounter].clear();
                }
            }
            else if(columnCounter==this->rowCount-1)
            {
                int colSize=(columnCounter%this->maxRowsPerBlock)+1;
                if(rowCounter%this->maxRowsPerBlock==this->maxRowsPerBlock-1)
                {
                    bufferManager.writePage(this->matrixName,this->blockCount,matrixsInPages[matrixCounter],min(rowCount,this->maxRowsPerBlock),colSize);
                    this->blockCount++;
                    this->perBlockCount.emplace_back(min(rowCount,this->maxRowsPerBlock),colSize);
                    matrixsInPages[matrixCounter].clear();
                }
                else if(rowCounter==this->rowCount-1)
                {
                    int rowSize=(rowCounter%this->maxRowsPerBlock)+1;
                    bufferManager.writePage(this->matrixName,this->blockCount,matrixsInPages[matrixCounter],rowSize,colSize);
                    this->blockCount++;
                    this->perBlockCount.emplace_back(rowSize,colSize);
                    matrixsInPages[matrixCounter].clear();
                }
            }
            else if(rowCounter%this->maxRowsPerBlock==this->maxRowsPerBlock-1 && columnCounter%this->maxRowsPerBlock==this->maxRowsPerBlock-1)
            {
                bufferManager.writePage(this->matrixName,this->blockCount,matrixsInPages[matrixCounter],min(rowCount,this->maxRowsPerBlock),min(rowCount,this->maxRowsPerBlock));
                this->blockCount++;
                this->perBlockCount.emplace_back(min(rowCount,this->maxRowsPerBlock),min(rowCount,this->maxRowsPerBlock));
                matrixsInPages[matrixCounter].clear();
            }
            columnCounter+=1;
            if(columnCounter==rowCount)
            {
                rowCounter++;
                columnCounter=0;
            }
            matrixCounter=columnCounter/this->maxRowsPerBlock;
        }
    }

    return true;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Matrix::transpose()
{
    for(int i=0;i<this->blockCount;i++)
    {
        int row=i/(int)(sqrt(this->blockCount));
        int column=i%(int)(sqrt(this->blockCount));
        // cout << i <<" "<<row<<" "<<column<<" "<<"\n";
        if(row==column)
        {
            int r=perBlockCount[i].first;
            int c=perBlockCount[i].second;
            Page p=bufferManager.getPage(this->matrixName,i,false);
            vector<vector<int>> data(c,vector<int> (r,0));
            for(int k=0;k<r;k++)
            {
                vector<int> row=p.getRow(k);
                for(int j=0;j<c;j++)
                {
                    data[j][k]=row[j];
                }
            }
            p.writePage(data,c,r);
            perBlockCount[i].first=c;
            perBlockCount[i].second=r;
        }
        else if(row<column)
        {
            int r1=perBlockCount[i].first;
            int c1=perBlockCount[i].second;
            Page p1=bufferManager.getPage(this->matrixName,i,false);
            vector<vector<int>> data1(c1,vector<int> (r1,0));
            for(int k=0;k<r1;k++)
            {
                vector<int> row=p1.getRow(k);
                for(int j=0;j<c1;j++)
                {
                    data1[j][k]=row[j];
                }
            }
            int in=column*(int)sqrt(this->blockCount)+row;
            int r2=perBlockCount[in].first;
            int c2=perBlockCount[in].second;
            // cout << r1 << " "<<c1 <<" "<< r2 <<" "<<c2<<"\n";
            Page p2=bufferManager.getPage(this->matrixName,in,false);
            vector<vector<int>> data2(c2,vector<int> (r2,0));
            for(int k=0;k<r2;k++)
            {
                vector<int> row=p2.getRow(k);
                for(int j=0;j<c2;j++)
                {
                    data2[j][k]=row[j];
                }
            }
            // cout << r1 << " "<<c1 <<" "<< r2 <<" "<<c2<<"\n";
            p1.writePage(data2,c2,r2);
            p2.writePage(data1,c1,r1);
            perBlockCount[i].first=c2;
            perBlockCount[i].second=r2;
            perBlockCount[in].first=c1;
            perBlockCount[in].second=r1;
        }
    }
    bufferManager.deleteFromPool(this->matrixName);
}

void Matrix::compute() 
{
    logger.log("Matrix::compute");
    string name=parsedQuery.computeMatrix+"_RESULT";
    Matrix *matrix = new Matrix(name, this->rowCount);
    matrix->blockCount=this->blockCount;
    matrixCatalogue.insertMatrix(matrix);
    vector<pair<int, int>> per_page_dim(matrix->blockCount);
    int dim = int(sqrt(this->blockCount));
    
    for (int di=0; di<blockCount; di+=dim+1) // compute of diagonal blocks
    {
        vector<vector<int>> p = bufferManager.getPage(this->matrixName, di, false).getRows();
        int n=p.size();
        for (int i=0; i<n; i++)
            p[i][i] = 0;
        
        for (int i=0; i<n; i++){
            for (int j=0; j<i; j++){
                p[i][j]-=p[j][i];
                p[j][i]=-p[i][j];
            }
        }
        bufferManager.writePage(name, di, p, n, n);
        per_page_dim[di]={n, n};
    }

    for (int pi=0;pi<this->blockCount;pi++)  // compute for non-diagonal blocks
    {
        int x = pi/dim;
        int y = pi%dim;
        if (x<y)
        {
            vector<vector<int>> p1 = bufferManager.getPage(this->matrixName, pi, false).getRows();
            vector<vector<int>> p2 = bufferManager.getPage(this->matrixName, y*dim+x, false).getRows();
            int n = p1.size();
            int m = p2.size();
            for (int i=0;i<n;i++)
            {
                for (int j=0;j<m;j++)
                {
                    p1[i][j]-=p2[j][i];
                    p2[j][i]=-p1[i][j];
                }
            }
            bufferManager.writePage(name, pi, p1, n, m);
            per_page_dim[pi]={n, m};
            bufferManager.writePage(name, y*dim+x, p2, m, n);
            per_page_dim[y*dim+x]={m, n};
        }
    }
    matrix->perBlockCount=per_page_dim;
}

bool Matrix::checksymmetry()
{
    int dim = int(sqrt(this->blockCount));
    int pi,i,j,n,m;
    for (pi=0; pi<blockCount; pi+=(dim+1)) // diagonal page blocks check
    {
        vector<vector<int>> p = bufferManager.getPage(this->matrixName, pi, false).getRows();
        n=p.size();
        for (i=1;i<n;i++){
            for (j=0;j<i;j++){
                if (p[i][j] != p[j][i])
                    return false;
            }
            // cout<<endl;
        }
    }
    // cout<<"Diag checked"<<endl;
    for (int pi=0; pi<blockCount; pi++) // non-diagonal page blocks check
    {
        i = pi/dim;
        j = pi%dim;
        if (i<j)
        {
            vector<vector<int>> p1 = bufferManager.getPage(this->matrixName, pi, false).getRows();
            vector<vector<int>> p2 = bufferManager.getPage(this->matrixName, j*dim+i, false).getRows();
            n = p1.size();
            m = p2.size();
            for (int i=0;i<n;i++)
            {
                for (int j=0;j<m;j++)
                {
                    if (p1[i][j] != p2[j][i])
                        return false;
                }
            }
        }
    }
    return true;
}
void Matrix::print()
{
    logger.log("Matrix::print");
    logger.log("Matrix::STATS "+to_string(this->rowCount)+":"+to_string(this->blockCount)+":"+to_string(this->maxRowsPerBlock));
    int count = min(PRINT_COUNT, this->rowCount);
    int total = count;
    int rowsleft = this->rowCount;
    // cout<<this->matrixName<<endl;
    Cursor cursor(this->matrixName, 0, false);
    for (int rowCounter = 0; rowCounter < total; rowCounter+=this->maxRowsPerBlock)
    {
        int numRows = (rowsleft>=this->maxRowsPerBlock)?this->maxRowsPerBlock:rowsleft;
        vector<vector<int>> result(numRows);
        int numLines = numRows*sqrt(this->blockCount);   
        for (int line=0;line < numLines;line++){
            vector<int> rowpart = cursor.getNext();
            int index = line%numRows;
            if (result[index].size()<20)
                result[index].insert(result[index].end(),rowpart.begin(),rowpart.end());
        }
        // cout<<"Count left "<<rowsleft<<":"<<count<<endl;
        if (count>numRows)
            this->writeRow(result, cout, true);
        else{
            vector<vector<int>> subResult(result.begin(), result.begin() + count);
            this->writeRow(subResult, cout, true);
        }
        count-=numRows;
        rowsleft-=numRows;
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
void Matrix::getNextPage(Cursor *cursor)
{
    logger.log("Matrix::getNext");
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
void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    // if(!this->isPermanent())
    //     bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    Cursor cursor(this->matrixName, 0, false);
    int rowsleft = this->rowCount;
    int total = rowsleft, numRows = this->maxRowsPerBlock;
    int numLines = (this->maxRowsPerBlock)*sqrt(this->blockCount);
    for (int rowCounter = 0; rowCounter < total; rowCounter+=this->maxRowsPerBlock)
    {
        if (rowsleft<this->maxRowsPerBlock)
        {
            numRows = rowsleft;
            numLines = numRows*sqrt(this->blockCount);  
        }
        vector<vector<int>> result(numRows);
        int index = 0;
        // vector<vector<int>::iterator> iterators(numRows);
        // for (auto& subVector: result)
        //     iterators[index++] = subVector.begin();
        // index = 0;

        for (int line=0;line < numLines;line++){
            vector<int> rowpart = cursor.getNext();
            // int index = line%numRows;
            if (result[index].empty())
                result[index].reserve(this->rowCount);
            
            for (const int& value:rowpart)
                result[index].push_back(value);
            // copy(rowpart.begin(),rowpart.end(), iterators[index]);
            // iterators[index] += rowpart.size();
            // result[index].insert(result[index].end(),rowpart.begin(),rowpart.end());
            index++;
            if (index==numRows)
                index = 0;
        }
        this->writeRow(result, fout, false);
        rowsleft-=numRows;
    }
    this->sourceFileName=newSourceFile;
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Matrix::unload(){
    logger.log("Matrix::~unload "+this->matrixName+" "+to_string(this->blockCount)+" blocks");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->matrixName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
// Cursor Matrix::getCursor()
// {
//     logger.log("Matrix::getCursor");
//     Cursor cursor(this->tableName, 0);
//     return cursor;
// }

/**
 * @brief Function to rename a matrix, and its pages
 * 
 * @return Cursor 
 */
bool Matrix::renameMatrix(string newName)
{
    logger.log("Matrix::rename");
    bufferManager.deleteFromPool(this->matrixName);
    // cout<<"Cleared "<<this->matrixName<<", "<<newName<<endl;
    for (int pageIndex = 0;pageIndex < blockCount;pageIndex++)
    {
        string oldFilename = "../data/temp/"+this->matrixName + "_Page" + to_string(pageIndex);
        string newFilename = "../data/temp/"+newName + "_Page" + to_string(pageIndex);
        
        if (rename(oldFilename.c_str(),newFilename.c_str())!=0)
            return false;
    }
    matrixCatalogue.deleteMatrix(this->matrixName);
    this->matrixName = newName;
    // logger.log("Matrix_rename::STATS "+to_string(this->rowCount)+":"+to_string(this->blockCount)+":"+to_string(this->maxRowsPerBlock));
    matrixCatalogue.insertMatrix(this);
    // logger.log("Matrix_rename_after::STATS "+to_string(this->rowCount)+":"+to_string(this->blockCount)+":"+to_string(this->maxRowsPerBlock));
    return true;
}