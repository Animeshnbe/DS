#include "global.h"

void MatrixCatalogue::insertMatrix(Matrix* matrix){
    logger.log("MatrixCatalogue::insertMatrix"); 
    this->matrices[matrix->matrixName] = matrix;
}
void MatrixCatalogue::deleteMatrix(string matrixName){
    logger.log("MatrixCatalogue::deleteMatrix"); 
    // this->matrices[matrixName]->unload();
    // delete this->matrices[matrixName];
    this->matrices.erase(matrixName);
}
Matrix* MatrixCatalogue::getMatrix(string matrixName){
    logger.log("MatrixCatalogue::getMatrix"); 
    Matrix *m = this->matrices[matrixName];
    return m;
}
bool MatrixCatalogue::isMatrix(string matrixName){
    logger.log("MatrixCatalogue::isMatrix"); 
    if (this->matrices.count(matrixName))
        return true;
    return false;
}
void MatrixCatalogue::print()
{
    logger.log("MatCatalogue::print"); 

    int rowCount = 0;
    cout<<"Currently in catalogue "<<endl;
    for (auto mat : this->matrices)
    {
        cout << mat.first << endl;
        rowCount++;
    }
    // printRowCount(rowCount);
}
MatrixCatalogue::~MatrixCatalogue(){
    logger.log("MatrixCatalogue::~MatrixCatalogue"); 
    for(auto m: this->matrices){
        m.second->unload();
        delete m.second;
    }
}
