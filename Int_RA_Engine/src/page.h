#include"logger.h"
/**
 * @brief The Page object is the main memory representation of a physical page
 * (equivalent to a block). The page class and the page.h header file are at the
 * bottom of the dependency tree when compiling files. 
 *<p>
 * Do NOT modify the Page class. If you find that modifications
 * are necessary, you may do so by posting the change you want to make on Moodle
 * or Teams with justification and gaining approval from the TAs. 
 *</p>
 */

enum PageType
{
    table,
    matrix
};

class Page{

    string tableName;
    string pageIndex;
    int columnCount;
    int rowCount;
    vector<vector<int>> rows;
    public:

    string pageName = "";
    
    Page();
    Page(string tableName, int pageIndex, bool isaTable=true);
    Page(string tableName, int pageIndex, vector<vector<int>> &rows, int rowCount, int colCount=-1);
    Page(string tableName, int pageIndex, vector<int> matrix, int rowCount,int colCount);
    vector<int> getRow(int rowIndex);
    vector<vector<int>> getRows();
    void writePage();
    void writePage(vector<int> &matrix);
    void writePage(vector<vector<int>> rows,int row,int column);
};