#include "InternalNode.hpp"
#include "RecordPtr.hpp"
#include "BPTree.hpp"
#include "stack"
#include "queue"
//creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr) {
    this->keys.clear();
    this->tree_pointers.clear();
    if (!is_null(tree_ptr))
        this->load();
}

vector<Key> & InternalNode::getKeys() {
    return this->keys;
}

vector<TreePtr> & InternalNode::getPointers() {
    return this->tree_pointers;
}

// map<Key, RecordPtr> &InternalNode::getMap(){}

//max element from tree rooted at this node
Key InternalNode::max() {
    Key max_key = DELETE_MARKER;
    TreeNode* last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

//if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr() {
    if (this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

TreePtr InternalNode::findLeftSibling(TreePtr parent) {
    if(is_null(parent)) return NULL_PTR;
    TreeNode *parentNode=TreeNode::tree_node_factory(parent);
    vector<TreePtr> &tp=parentNode->getPointers();
    auto it = tp.begin();
    while((*it)!=this->tree_ptr)
        it=next(it);
    if(it==tp.begin())
        return NULL_PTR;
    delete parentNode;
    return *prev(it);
}

TreePtr InternalNode::findRightSibling(TreePtr parent) {
    if(is_null(parent)) return NULL_PTR;
    TreeNode *parentNode=TreeNode::tree_node_factory(parent);
    vector<TreePtr> &tp=parentNode->getPointers();
    auto it = tp.begin();
    while((*it)!=this->tree_ptr)
        it=next(it);
    if(next(it)==tp.end())
        return NULL_PTR;
    delete parentNode;
    return *next(it);
}

//inserts <key, record_ptr> into subtree rooted at this node.
//returns pointer to split node if exists
//TODO: InternalNode::insert_key to be implemented
TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr, TreePtr parent) {
    // cout<<endl<<"at internal node "<<this->tree_ptr<<" to insert "<<key;
    TreePtr new_tree_ptr = NULL_PTR;
    vector<Key> &keys=this->getKeys();
    vector<TreePtr> &ptrs=this->getPointers();
    int pos=0;
    while(pos<keys.size() && keys[pos] < key) {
        pos++;
    }
    TreeNode* nextNode = TreeNode::tree_node_factory(ptrs[pos]);
    // cout<<" Next node "<<ptrs[pos]<<endl;
    TreePtr newSplit = nextNode->insert_key(key, record_ptr, this->tree_ptr);
    // cout<<endl<<"inserted in "<<ptrs[pos]<< " returned "<<newSplit<<endl;
    //find siblings
    TreePtr leftSibling=this->findLeftSibling(parent);
    // cout<<"\nleft sibling found\n";
    TreePtr rightSibling=this->findRightSibling(parent);
    // cout<<"\nright sibling found\n";

    // updating keys after redistribution to left
    if(pos>0) {
        TreeNode *leftNode=TreeNode::tree_node_factory(this->tree_pointers[pos-1]);
        keys[pos-1]=leftNode->max();
        delete leftNode;
    }
    // cout<<"\nUpdate 1\n";
    // updating keys after redistribution to right
    // TreeNode *rightestOfCurr=TreeNode::tree_node_factory(this->tree_pointers.back());
    nextNode = TreeNode::tree_node_factory(ptrs[pos]);
    keys[pos]=nextNode->max();
    // cout<<"\nUpdate 2\n";


    if(!is_null(newSplit)) {
        TreeNode* splitedNode = TreeNode::tree_node_factory(newSplit);
        Key maxKeyOfNewSplit=splitedNode->max();
        TreeNode* nextNode = TreeNode::tree_node_factory(ptrs[pos]);
        keys[pos]=nextNode->max();
        keys.insert(keys.begin()+pos+1, maxKeyOfNewSplit);
        ptrs.insert(ptrs.begin()+pos+1, newSplit);
        this->size++;
        if(this->overflows()) {
            bool inserted=false;
            // if(!is_null(leftSibling)) {
            //     TreeNode *leftNode=TreeNode::tree_node_factory(leftSibling);
            //     if(!leftNode->is_full()) {
            //         // redistribute to left sibling
            //         auto &leftKeys = leftNode->getKeys();
            //         auto &leftPtrs = leftNode->getPointers();
            //         TreeNode *rightestOfLeft=TreeNode::tree_node_factory(leftPtrs.back());
            //         leftKeys.push_back(rightestOfLeft->max());
            //         delete rightestOfLeft;
            //         leftPtrs.push_back(this->tree_pointers[0]);
            //         leftNode->size++;
            //         this->size--;
            //         this->keys.erase(this->keys.begin());
            //         this->tree_pointers.erase(this->tree_pointers.begin());
            //         inserted=true;
            //     }
            //     leftNode->dump();
            //     delete leftNode;
            // }
            // if(!inserted && !is_null(rightSibling)) {
            //     TreeNode *rightNode=TreeNode::tree_node_factory(rightSibling);
            //     if(!rightNode->is_full()) {
            //         // redistribute to right sibling
            //         auto &rightKeys = rightNode->getKeys();
            //         auto &rightPtrs = rightNode->getPointers();
            //         rightPtrs.insert(rightPtrs.begin(), this->tree_pointers.back());
            //         TreeNode *rightestOfCurr=TreeNode::tree_node_factory(this->tree_pointers.back());
            //         rightKeys.insert(rightKeys.begin(), rightestOfCurr->max());
            //         this->tree_pointers.pop_back();
            //         this->keys.pop_back();
            //         rightNode->size++;
            //         this->size--;
            //         delete rightestOfCurr;
            //         inserted=true;
            //     }
            //     rightNode->dump();
            //     delete rightNode;
            // }
            if(!inserted) {
                //have to split
                TreeNode *newNode = TreeNode::tree_node_factory(INTERNAL);
                auto &newKeys = newNode->getKeys();
                auto &newPtrs = newNode->getPointers();
                newKeys=vector<Key> (this->keys.begin()+MIN_OCCUPANCY, this->keys.end());
                newPtrs=vector<TreePtr> (this->tree_pointers.begin()+MIN_OCCUPANCY, this->tree_pointers.end());
                this->keys.erase(this->keys.begin()+MIN_OCCUPANCY-1, this->keys.end());
                this->tree_pointers.erase(this->tree_pointers.begin()+MIN_OCCUPANCY, this->tree_pointers.end());
                newNode->size=this->size-MIN_OCCUPANCY;
                this->size=MIN_OCCUPANCY;
                new_tree_ptr=newNode->tree_ptr;
                newNode->dump();
                delete newNode;
            }
        }
    }
    this->dump();
    return new_tree_ptr;
}

//deletes key from subtree rooted at this if exists
//TODO: InternalNode::delete_key to be implemented
void InternalNode::delete_key(const Key &key, TreePtr parent) {
    vector<Key> &keys=this->getKeys();
    vector<TreePtr> &ptrs=this->getPointers();
    int pos=0;
    while(pos<keys.size() && key > keys[pos]) {
        pos++;
    }
    
    TreeNode* nextNode = TreeNode::tree_node_factory(ptrs[pos]);
    // cout<<"Key: "<<key<<", At this node ptr is "<<this->tree_ptr<<endl;
    // cout<<"Pos = "<<pos<<"; "<<nextNode->tree_ptr<<endl;
    // cout<<" Next node "<<ptrs[pos]<<endl;
    nextNode->delete_key(key, this->tree_ptr);
    // cout<<"In "<<this->tree_ptr<<": "<<pos<<endl;
    // cout<<"Deleted from "<<ptrs[pos]<<endl;
    // cout<<endl<<"inserted in "<<ptrs[pos]<< " returned "<<newSplit<<endl;
    //find siblings
    TreePtr leftSibling=this->findLeftSibling(parent);
    TreePtr rightSibling=this->findRightSibling(parent);
    // updating keys after redistribution to left
    if(pos>0) {
        TreeNode *leftNode=TreeNode::tree_node_factory(this->tree_pointers[pos-1]);
        keys[pos-1]=leftNode->max();
        // cout<<"Assigned "<<keys[pos-1]<<endl;
        delete leftNode;
    }

    nextNode = TreeNode::tree_node_factory(ptrs[pos]);
    
    TreePtr prev = ptrs[pos];
    if (nextNode->size==0){
        if (pos == keys.size())
            keys.erase(keys.begin()+(pos-1));
        else
            keys.erase(keys.begin()+pos);
        ptrs.erase(ptrs.begin()+pos);
        // cout<<"Rem "<<ptrs.size()<<endl;
        this->size--;
        // cout<<"Now "<<this->size<<endl;
    }
    if (pos<keys.size() && ptrs[pos]==prev){
        keys[pos]=nextNode->max();
    }

    if(this->underflows()) {
        // cout<<"Underflow!"<<endl;
        if (leftSibling != NULL_PTR){
            TreeNode *leftNode = TreeNode::tree_node_factory(leftSibling);
            auto& leftPtrs = leftNode->getPointers();
            auto& leftKeys = leftNode->getKeys();
            if(leftNode->size > MIN_OCCUPANCY){
                //take from left sibling
                ptrs.insert(ptrs.begin(),leftPtrs.back());
                keys.insert(keys.begin(),leftNode->max());
                leftPtrs.pop_back();
                leftKeys.pop_back();
                leftNode->size--;
                this->size++;
            } else {
                // merge with left
                TreeNode *lastNode = TreeNode::tree_node_factory(leftPtrs.back());
                // cout<<"Taking "<<lastNode->max()<<" inside "<<leftKeys.back()<<endl;

                leftKeys.push_back(lastNode->max());
                // leftPtrs.push_back(ptrs[0]);
                if (!keys.empty())
                    leftKeys.insert(leftKeys.end(),keys.begin(),keys.end());
                leftPtrs.insert(leftPtrs.end(),ptrs.begin(),ptrs.end());
                leftNode->size += this->size;
                // cout<<"Now "<<leftNode->size<<endl;
                this->size = 0;
                // delete this node
            }
            leftNode->dump();
            delete leftNode;
        } else if (rightSibling != NULL_PTR) {
            TreeNode *rightNode = TreeNode::tree_node_factory(rightSibling);
            auto& rightPtrs = rightNode->getPointers();
            auto& rightKeys = rightNode->getKeys();
            if(rightNode->size > MIN_OCCUPANCY){
                //take from right sibling
                ptrs.push_back(*(rightPtrs.begin()));
                keys.push_back(this->max());
                rightPtrs.erase(rightPtrs.begin());
                rightKeys.erase(rightKeys.begin());
                rightNode->size--;
                this->size++;
            } else {
                // merge with right
                // cout<<"Taking "<<this->max()<<" inside "<<rightKeys[0]<<endl;
                rightKeys.insert(rightKeys.begin(), this->max());

                if (!keys.empty())
                    rightKeys.insert(rightKeys.begin(),keys.begin(),keys.end());
                else
                    // cout<<"Yaya"<<endl;
                rightPtrs.insert(rightPtrs.begin(),ptrs.begin(),ptrs.end());
                rightNode->size += this->size;
                // cout<<"New size of right "<<rightNode->size<<endl;
                this->size = 0;
            }
            rightNode->dump();
            delete rightNode;
        } else {
            // root
        }
    }
    // cout << "InternalNode::delete_key not implemented" << endl;
    this->dump();
    // cout<<"Dump success in "<<this->tree_ptr<<endl;
}

//runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for (int i = 0; i < this->size - 1; i++) {
        if (min_key <= this->keys[i]) {
            auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child_node->range(os, min_key, max_key);
            delete child_node;
            return;
        }
    }
    auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    child_node->range(os, min_key, max_key);
    delete child_node;
}

//exports node - used for grading
void InternalNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for (int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for (int i = 0; i < this->size; i++) {
        auto child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        child_node->export_node(os);
        delete child_node;
    }
}

//writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for (int i = 0; i < this->size; i++) {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if (i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1) {
            link += to_string(this->keys[i - 1]) + " < x";
        } else {
            link += to_string(this->keys[i - 1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream& InternalNode::write(ostream &os) const {
    TreeNode::write(os);
    // cout<<size<<":"<<this->tree_pointers.size()<<endl;
    if (size==0) return os;
    for (int i = 0; i < this->size - 1; i++) {
        if (&os == &cout)
            os << "\nP" << i + 1 << ": ";
        os << this->tree_pointers[i] << " ";
        if (&os == &cout)
            os << "\nK" << i + 1 << ": ";
        os << this->keys[i] << " ";
    }
    if (&os == &cout)
        os << "\nP" << this->size << ": ";
    os << this->tree_pointers[this->size - 1];
    return os;
}

istream& InternalNode::read(istream& is) {
    TreeNode::read(is);
    if (size==0) return is;
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
    for (int i = 0; i < this->size - 1; i++) {
        if (&is == &cin)
            cout << "P" << i + 1 << ": ";
        is >> this->tree_pointers[i];
        if (&is == &cin)
            cout << "K" << i + 1 << ": ";
        is >> this->keys[i];
    }
    if (&is == &cin)
        cout << "P" << this->size;
    is >> this->tree_pointers[this->size - 1];
    return is;
}
