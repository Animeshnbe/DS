// #include <bits/stdc++.h>
// using namespace std;

// int main(){
//     // vector<vector<int>> mat = {
//     //     {29,48,94,9,27},
//     //     {60,11,29,34,45},
//     //     {39,40,49,78,62},
//     //     {53,90,90,59,41},
//     //     {53,57,42,61,89},
//     //     {8,53,98,70,48},
//     //     {45,78,57,82,87},
//     //     {55,84,96,50,75},
//     //     {39,35,72,88,74},
//     //     {53,54,83,95,33},
//     //     {43,32,90,39,95},
//     //     {96,78,41,52,86},
//     //     {31,19,16,79,36},
//     //     {10,63,46,27,63},
//     //     {39,13,95,15,28}
//     // };

//     vector<vector<int>> mat = {
//         {27},
//         {16},
//         {5},
//         {9},
//         {2},
//         {23},
//         {19},
//         {11},
//         {1},
//         {6},
//         {8},
//         {1},
//         {22},
//         {14},
//         {25},
//         {10},
//         {21},
//         {12},
//         {7},
//         {18},
//         {24},
//         {4},
//         {13},
//         {1},
//         {20},
//         {17},
//         {3},
//         {15},
//         {26}
//     };

//     // for (auto& v:mat){
//     //     for (int& e:v)
//     //         cout<<e<<" ";
//     //     cout<<endl;
//     // }

//     vector<pair<int,bool>> cols(1);
//     cols[0] = {0,true};
//     auto comp = [cols](vector<int>& a, vector<int>& b) {
//         for (int i = 0; i < cols.size(); i++) {
//             bool ord = cols[i].second;
//             int ind = cols[i].first;
//             if (a[ind] == b[ind]) continue;
//             if (ord) {
//                 if (a[ind] < b[ind]) return true;
//                 else if (a[ind] > b[ind]) return false;
//             } else {
//                 if (a[ind] > b[ind]) return true;
//                 else if (a[ind] < b[ind]) return false;
//             }
//         }
//         return true;
        
//     };

//     sort(mat.begin(),mat.end(),comp);

//     int i=1;
//     for (auto& v:mat){
//         cout<<i++<<": ";
//         for (int& e:v)
//             cout<<e<<" ";
//         cout<<endl;
//     }
// }