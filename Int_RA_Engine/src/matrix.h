/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */

class Matrix
{
public:
    string sourceFileName = "";
    string matrixName = "";
    unsigned int rowCount = 0;
    unsigned int blockCount = 0;
    unsigned int maxRowsPerBlock = 0;
    vector<pair<int,int>> perBlockCount;
    // bool indexed = false;
    // string indexedColumn = "";
    // IndexingStrategy indexingStrategy = NOTHING;
    bool blockify();
    // void updateStatistics(vector<int> row);
    Matrix(string matrixName);
    Matrix(string matrixName, int n);
    bool load();
    void print();
    bool renameMatrix(string newName);
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    void transpose();
    void compute();
    bool checksymmetry();
    Cursor getCursor();
    void unload();

        /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row 
     */
    template <typename T>
    void writeRow(vector<vector<T>> row, ostream &fout, bool console)
    {
        logger.log("Matrix::printRows");

        for (int rowCounter = 0; rowCounter < row.size(); rowCounter++)
        {
            if (console)
            {
                for (int columnCounter = 0; columnCounter < min(20,int(row[0].size())); columnCounter++)
                {
                    if (columnCounter != 0)
                        fout << ", ";
                    fout << row[rowCounter][columnCounter];
                }
            } else
            {
                for (int columnCounter = 0; columnCounter < row[0].size(); columnCounter++)
                {
                    if (columnCounter != 0)
                        fout << ", ";
                    fout << row[rowCounter][columnCounter];
                }
            }
            fout << endl;
        }
    }

    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row 
     */
    template <typename T>
    void writeRow(vector<T> row)
    {
        logger.log("Matrix::printRow");
        // ofstream fout(this->sourceFileName, ios::app);
        // this->writeRow(row, fout);
        // fout.close();
    }
};
