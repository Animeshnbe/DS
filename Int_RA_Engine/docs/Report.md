# PHASE 2
## OVERVIEW

We implement external sorting on the rows (vector<int>) according to a set order of columns described in the query. For this we are constrained to read only at max 9 blocks at a time and can keep a single output block which can be written at a time. 

## Detailed Description
We run a 3-level nested iteration, first over the number of levels of merge-sort, then over the number of chunks or runs to be sorted, and finally over the blocks read from the run. We read 9 blocks from the run at a time and if it is at level 0, we sort them individually using our custom comparator.

    Custom comparator: 
        auto comp = [this](const vector<int>& a, const vector<int>& b) {
            // cols is a vector of pairs, containing the respective column and
            // its order (ASC/DESC) in order of the query specification.
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

<br/> 
    
    Iterations:
        int dm = 9; //degree of merge
        int lvl = 0;
        priority_queue pq; //with custom comparator
        Loop 1: do {
            pi = 0;
            Loop 2: for (int i=0; i<blockCount; i+=pow(dm,(lvl+1))){ // run
                spages.clear();
                
                Loop 3a: for (int j=0; j<pow(dm,(lvl+1)); j+=pow(dm,lvl)){
                    ...
                    < sort blocks if lvl==0 >
                    < push first row pointer to pq >
                }
    
                Loop 3b: while (!pq.empty()){
                    ... <write buffer if full>
                    ... <read next block on run if any>
                }
    
                ... <write any remaining buffer>
                ... <delete all blocks in current run>
            }
            ... <rename blocks>
            lvl++;
        } while (pow(dm,lvl)<blockCount);

We push the top row of each block to priority queue and keep track of the current 9 blocks' record pointers in an unordered_map (of max size 9).
+ While the priority queue is not empty, we retrieve the top row pointer and write the data to the buffer and move the pointer ahead in the corresponding block.
+ If the pointer reaches the end of the block, we read the next block in the run (if any) and push its rows to our map in place of the old block.
+ If the buffer turns full upto the maximum number of rows, we flush it to the output pages marked by the level number and block sequence number.
+ After each run we delete all page blocks in the current run as it is sorted.
+ After each level we rename all the output page blocks to remove the level number and it becomes the page from where the next level will be processed.

## Join
>We initialize a new table object with the metadata calculated according to metadata of the two tables on which the join operation is performed. We push the new rows formed by joining rows of the joining tables whenever condition is fulfilled.
+ We sort both tables by the respective columns to be joined on, and store these to temporary page blocks.
+ We set a cursor to the top rows of each table.
+ Based on the condition, we declare a temporary cursor and iterate over the temporary cursor until the condition is true and then move to the next row of the other table and write the output buffer to a new page whenever required.

## Order by
> We initialise a new table object with the same meta data as the table to be sorted. Then we push the sorting attribute and its order to a vector of size 1, the same generic vector we created for external sorting.
+ Then we call the same external sorting function with a inplace boolean flag set to false.
+ By checking this flag, we make sure not to delete any page block in the runs of the level 0, so the original table is preserved.
+ At the end of iterating over all levels, we insert the new table to tableCatalogue.

## Group by
> We sort the table to a temporary table on the column by which it is to be grouped. Then we set current values of the aggregate condition to be checked as well as the aggregate attribute to be returned, according to the respective aggregations (e.g. 0 for sum, INT_MIN for max, etc). Then we iterate through all rows in the table.
+ While the value on the grouping attribute is same, we keep updating both the condition and return values. 
+ When the grouping value changes, we filter based on the condition value, and if true, we insert a row with the grouping value and return value. We reset the initial condition and return value.
+ If the write buffer is full, we write a new page.

### Assumptions
The tables to be joined should not have # in the name as temporary blocks at each level append the level number with # in the page name. All data within the tables should be integers without formatting, and not empty, so as load and print functions properly and query is tokenized correctly. Aggregate operations should not overflow or underflow over integer ranges.

### Contributions
> Animesh Das: 33.33% semantic and syntax parsing, sort comparator, Order by \
> Ritvik Gupta: 33.33% Sort merge runs and in-place assignment, Join Query \
> Sumonto Chatterjee: 33.33% Aggregate function, Group By and large table testing.

All members coordinated on Priority queue based algorithm design, dry runs, and report compilation.

## LEARNINGS
All data structures and classes are designed with optimal access and constraints in mind, such that accessing a specific row and a value within the row takes less index computations. Suitable syntactic and semantic checks are implemented for each query, and strict adherence to the column and order is followed, using regex as and when necessary. In the course of the external sorting phase we limited all of the sort and parse operations to 1 read/write access per block, including the sort runs within a block. The flow of information between the various data structures defined, as well as any exceptional scenarios such as empty results and conditions are handled. All constructor parameter passing is controlled to make sure copies of the data members are made wherever required instead of pass by reference.

We analyze the various time and space complexities based on the logical operators on the attributes (>, <, ==, etc.) and find that join takes O(n1\*n2) on the worst case where n1 and n2 are the row sizes of the two tables respectively. External sorting itself takes n\*log(n) time and a buffer space of 10 blocks. Group by also takes O(n) additional time using the cursor in the worst case since a running value of sum, count, max, min, or average is kept. We also learned making custom comparators for duplicate data as well, and file handling for renaming the page blocks suitably for excluding stale reads. 

# PHASE 1
## OVERVIEW:
The project phase 1 focuses on RA handling matrices (storage, retrieval and projection) with some additional basic operations. To handle matrices alongside tables, we mostly need to overcome the column size constraints and handle for the non-existing column-headers, as well as optimize retrieval and write accesses suited to matrix properties.

## PAGE DESIGN:
We do not reinvent the wheel-- we use the same page design (page.h Page class), but with an additional constructor to specify the column count of the page, since it is not constant. The page fetch constructor also contains a **isaTable** flag to fetch the metadata from the appropriate catalogue before reading the page.

![design](https://github.com/DSMonsoon23/initial-set-up-team-17/assets/35167280/23574fcc-056a-495a-bf94-62ef65a819ef)


We also use the same cursor and buffermanager to iterate rows and maintain the cache. 
The only changes are:
1. Cursor: constructor contains the **isaTable** flag to use the correct catalogue while querying for next page. 
2. Buffermanager: An overloaded writePage containing a specific column count and a new deleteFromPool method to invalidate the pages after rename or transpose in-place. insertIntoPool also contains a new parameter isaTable to pass on to the Page constructor.
3. ParsedQuery: isaTable flag to reuse semantic parsing and execute blocks for load, print, rename and export.

## OTHER MAJOR CHANGELOGS:
We use the **isaTable** flag also to reuse the same syntactic and semantic parser for LOAD, PRINT, EXPORT and RENAME as was defined for table. The semantic checks are done appropriately, handling errors gracefully for any non-existent matrices or invalid arguments.

### Optimization, \# of accesses
For optimal iteration performance and lookups, we compute the total number of page blocks and # of page blocks per row in advance and also use the fact that the matrix is **square** to our advantage. We also suitably break from the iterations in print operation and write only the actual elements to Page blocks, without any padding.

We contruct pages of approximately 1KB which can store upto 16*16=256 integers (+ve or -ve). This is calculated as follows: sqrt(1024/sizeof(int)). The same page size and deque size of buffer (=2) is used from the table storage design. The number of pages is given by **blockCount** and we observe that it will always be a perfect square.

For print and export, the pages are read sequentially when the cursor reaches the end of current page, thus accessing ceil(size/16)*size number of cursor accesses. For transpose, checksymmetry and compute, number of page accesses is equal to **blockCount**, but the order of access is optimized to reduce buffer memory. Further details on the operations 

### Assumptions
- We assume the pages in data/temp is a protected, tamper-proof space reserved by the application and as such consistency checks on the catalog is not necessary.
- We assume that sufficient heap memory is available to hold a long catalogue, and bufferManager can maintain a deque of 2 pages at least.
- We assume that valid square matrix .csv's are provided in the input, and characters can be tokenized to integers without overflow (-2^31-1 to +2^31). We also assume that compute operation won't result in subtraction overflow.
- We assume that the .csv files will not be removed after loading and/or matrices be exported before quitting, else the matrices will be lost permanently.
- We assume that matrix and table with same name will not be loaded.

Maximum number of rows/columns per page = mRB = $$\sqrt{1024/sizeof(int)}$$

LOAD
> We load a matrix by initializing the matrix object with name, size and reading in chunks of maxRowsperblock from the .csv file. Then we use the writePage method of the bufferManager (overloaded) to write the page blocks with indexed names. Upon loading, matrixname and file name is same, and the object with metadata is pushed into the matrix catalogue's unordered_map, indexed by the name only.\
_Syntax_: ```LOAD MATRIX <FILENAME>```\
> \# Write Access = $${ceil(n/mRB)^2}$$

PRINT
> We print the top-left 20*20 cells of the matrix by fetching the required number of lines using the cursor. Since the page blocks were written in row-major order, the submatrix buffers are also created in row-major and appended to the result matrix.\
_Syntax_: ```PRINT MATRIX <FILENAME>```\
> \# Read Access = $${min(ceil(20/mRB)^2,ceil(n/mRB)^2)}$$

RENAME
> We rename the matrix in 3 steps:
> - first delete the pages of the matrix in buffer.
> - rename the pages of the matrix
> - remove the old matrix from catalogue
> - reassign the *matrixName* and push back into the catalogue. \
_Syntax_: ```RENAME MATRIX <OLDNAME> <NEWNAME>```\
> \# Page name Access = $${ceil(n/mRB)^2}$$

EXPORT
> We export in the same manner as PRINT, by accessing all the lines in all pages with cursor (instead of just 20). This sometimes takes longer for the sequential access of cursor, for matrices larger than 5K*5K. \
_Syntax_: ```EXPORT MATRIX <FILENAME>```\
> \# Read Access = $${ceil(n/mRB)^2}$$

CHECKSYMMETRY
> We run a loop first on the diagonal blocks (one by one) and then on non-diagonal blocks (2 at a time, pageIndex of the second is computed from the first). We check the symmetry trivially on diagonal blocks, and compare the opposing elements on the non-diagonal blocks. In case of a mismatch the function returns FALSE immediately. If all the checks are successful, it returns TRUE. \
_Syntax_: ```CHECKSYMMETRY <MATRIXNAME>```\
> \# Read Access =  $${O(ceil(n/mRB)^2)}$$

TRANSPOSE
> We run a loop on the blocks and if the block is present on the principal diagonal then just transpose the elements of the block.In other case for a given block we calculate the block it must be swapped with to perform transpose.To perform this operation in-place we calculate transpose of blocks in pairs and then write back data into blocks(of their counterpart).The memory used other than the blocks is of size of 2 pages thus the operation is in-place. \
_Syntax_: ```TRANSPOSE MATRIX <MATRIXNAME>```\
> \# Read Access = $${ceil(n/mRB)^2}$$
> \# Write Access = $${ceil(n/mRB)^2}$$

COMPUTE
> We run a loop first on the diagonal blocks and then on non-diagonal blocks similar to checksymmetry to fetch only the required pages for operations. We also utilise the property of the operations to reduce the arithmetic operations to only the lower triangular part of the matrix. We initialize the result matrix with appropriate dimensions and write the required pages with indexing through the _Buffermanager_.\
_Syntax_: ```COMPUTE <MATRIXNAME>```\
> \# Read Access = $${ceil(n/mRB)^2}$$
> \# Write Access = $${ceil(n/mRB)^2}$$

## LEARNINGS
All storage techniqies and OOP structures are designed with optimal access and constraints in mind. An analysis of the calls and access modifiers reveals the whole rationale behind adopting this pattern. In the course of the project we discovered various calculations on read/write access of blocks and flow of information between the various data structures defined, as well as any exceptional scenarios and redundancies put in place to reduce the latency and assumptions of operations.\
We also observed that dividing the page into blocks that mimic the structure of the whole data (matrix) increases the efficiency of the iterations to access the lines with minimal modulo arithmetic operations, which are expensive at scale. Hence we adopted pages that contain square matrices maximally. Although this means there is a small fragmentation tradeoff for the pages on the edge and bottom right, the time optimization more than compensates for our use case.\
We plan to also extend the unload functionality of matrices to clear a specific matrix in catalogue memory as well, to allow for extensive testing of commands.

### Contributions
Animesh ([Animeshnbe](https://github.com/Animeshnbe)) - 33.33% \
Ritvik ([ritvik369](https://github.com/ritvik369)) - 33.33% \
Sumonto ([sumonto41](https://github.com/sumonto41)) - 33.33%
