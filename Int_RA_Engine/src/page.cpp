#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string tableName, int pageIndex, bool isaTable)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    unsigned int maxRowCount;
    if (isaTable){
        Table table = *tableCatalogue.getTable(tableName);
        this->columnCount = table.columnCount;
        this->rowCount = table.rowsPerBlockCount[pageIndex];
        maxRowCount = table.maxRowsPerBlock;
        vector<int> row(columnCount, 0);
        this->rows.assign(maxRowCount, row);
    }
    else{
        Matrix matrix = *matrixCatalogue.getMatrix(tableName);
        this->columnCount = matrix.perBlockCount[pageIndex].second;
        this->rowCount = matrix.perBlockCount[pageIndex].first;
        vector<int> row(columnCount, 0);
        this->rows.assign(rowCount, row);
    }
    
    ifstream fin(pageName, ios::in);
    int number;
    for (unsigned int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
    blocksRead++;
}

/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}

vector<vector<int>> Page::getRows()
{
    logger.log("Page::getRows");
    return this->rows;
}

Page::Page(string tableName, int pageIndex, vector<vector<int>> &rows, int rowCount, int colCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    if (colCount==-1)
        this->columnCount = rows[0].size();
    else
        this->columnCount = colCount;
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
}

Page::Page(string tableName, int pageIndex, vector<int> matrix, int rowCount, int colCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rowCount = rowCount;
    this->columnCount = colCount;
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writePage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
    blocksWritten++;
}
void Page::writePage(vector<vector<int>> rows,int  row,int column)
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < row; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < column; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
    blocksWritten++;
}
void Page::writePage(vector<int> &matrix)
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << matrix[rowCounter*this->columnCount+columnCounter];
        }
        fout << endl;
    }
    fout.close();
    blocksWritten++;
}
