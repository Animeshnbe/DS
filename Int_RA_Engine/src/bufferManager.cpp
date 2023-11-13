#include "global.h"

BufferManager::BufferManager()
{
    logger.log("BufferManager::BufferManager");
}

/**
 * @brief Function called to read a page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param name 
 * @param pageIndex 
 * @return Page 
 */
Page BufferManager::getPage(string name, int pageIndex, bool isaTable)
{
    logger.log("BufferManager::getPage "+name+"_Page" + to_string(pageIndex));
    string pageName;
    pageName = "../data/temp/"+name + "_Page" + to_string(pageIndex);
    // else
    //     pageName = "../data/temp/"+name + "_Page" + to_string(pageIndex) + "_" + to_string(pageColIndex);
    if (this->inPool(pageName))
        return this->getFromPool(pageName);
    else
        return this->insertIntoPool(name, pageIndex, isaTable);
}

/**
 * @brief Checks to see if a page exists in the pool
 *
 * @param pageName 
 * @return true 
 * @return false 
 */
bool BufferManager::inPool(string pageName)
{
    logger.log("BufferManager::inPool "+to_string(this->pages.size()));
    for (auto page : this->pages)
    {
        if (pageName == page.pageName)
            return true;
    }
    return false;
}

/**
 * @brief If the page is present in the pool, then this function returns the
 * page. Note that this function will fail if the page is not present in the
 * pool.
 *
 * @param pageName 
 * @return Page 
 */
Page BufferManager::getFromPool(string pageName)
{
    logger.log("BufferManager::getFromPool");
    for (auto page : this->pages)
        if (pageName == page.pageName)
            return page;
}

/**
 * @brief Inserts page indicated by tableName and pageIndex into pool. If the
 * pool is full, the pool ejects the oldest inserted page from the pool and adds
 * the current page at the end. It naturally follows a queue data structure. 
 *
 * @param tableName 
 * @param pageIndex 
 * @return Page 
 */
Page BufferManager::insertIntoPool(string tableName, int pageIndex, bool isaTable)
{
    logger.log("BufferManager::insertIntoPool");
    Page page(tableName, pageIndex, isaTable);
    if (this->pages.size() >= BLOCK_COUNT)
        pages.pop_front();
    pages.push_back(page);
    return page;
}

/**
 * @brief Deletes all pages in pool indicated by name. 
 *
 * @param name 
 */
void BufferManager::deleteFromPool(string name){
    logger.log("BufferManager::deleteFromPool "+name);
    auto it = pages.begin();

    name.push_back('_'); 
    while (it != pages.end()) {
        if ((it->pageName).find(name)!=string::npos)
            it = pages.erase(it);
        else
            ++it;
    }
    logger.log("BufferManager::After delete size "+to_string(this->pages.size()));
}
/**
 * @brief The buffer manager is also responsible for writing pages. This is
 * called when new tables are created using assignment statements.
 *
 * @param tableName 
 * @param pageIndex 
 * @param rows 
 * @param rowCount 
 */
void BufferManager::writePage(string name, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("BufferManager::writePage");
    Page page(name, pageIndex, rows, rowCount);
    page.writePage();
}


void BufferManager::writePage(string name, int pageIndex, vector<vector<int>> &rows, int rowCount, int colCount)
{
    // matrix
    logger.log("BufferManager::writePage");
    Page page(name, pageIndex, rows, rowCount, colCount);
    page.writePage();
}

void BufferManager::writePage(string name, int pageIndex, vector<int> &matrix, int rowCount, int colCount)
{
    logger.log("BufferManager::writePage");
    Page page(name, pageIndex, matrix, rowCount, colCount);
    page.writePage(matrix);
}

/**
 * @brief Deletes file names fileName
 *
 * @param fileName 
 */
void BufferManager::deleteFile(string fileName)
{
    
    if (remove(fileName.c_str()))
        logger.log("BufferManager::deleteFile: Err");
        else logger.log("BufferManager::deleteFile: Success");
}

/**
 * @brief Overloaded function that calls deleteFile(fileName) by constructing
 * the fileName from the tableName and pageIndex.
 *
 * @param tableName 
 * @param pageIndex 
 */
void BufferManager::deleteFile(string tableName, int pageIndex)
{
    logger.log("BufferManager::deleteFile");
    string fileName;
    // if (pageColIndex==-1)
    fileName = "../data/temp/"+tableName + "_Page" + to_string(pageIndex);
    // else
    //     fileName = "../data/temp/"+tableName + "_Page" + to_string(pageIndex) + "_" + to_string(pageColIndex);
    this->deleteFile(fileName);
}