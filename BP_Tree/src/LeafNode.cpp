#include "RecordPtr.hpp"
#include "LeafNode.hpp"

LeafNode::LeafNode(const TreePtr &tree_ptr) : TreeNode(LEAF, tree_ptr) {
    this->data_pointers.clear();
    this->next_leaf_ptr = NULL_PTR;
    if(!is_null(tree_ptr))
        this->load();
}

//returns max key within this leaf
Key LeafNode::max() {
    auto it = this->data_pointers.rbegin();
    return it->first;
}

//get mapping
map<Key, RecordPtr> & LeafNode::getMap() {
    return this->data_pointers;
}

void LeafNode::set_next_leaf(TreePtr tp) {
    this->next_leaf_ptr = tp;
}

TreePtr LeafNode::findLeftSibling(TreePtr parent) {
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

TreePtr LeafNode::findRightSibling(TreePtr parent) {
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

//inserts <key, record_ptr> to leaf. If overflow occurs, leaf is split
//split node is returned
//TODO: LeafNode::insert_key to be implemented
TreePtr LeafNode::insert_key(const Key &key, const RecordPtr &record_ptr, TreePtr parent) {
    // cout<<endl<<"at leaf node "<<this->tree_ptr<<" to insert "<<key<<endl;
    TreePtr new_leaf = NULL_PTR; // for new leaf is split happens
    if(this->data_pointers.find(key)!=this->data_pointers.end()) // key already exists
        return NULL_PTR;
    this->data_pointers[key]=record_ptr;
    this->size++;
    if(this->overflows()) {
        TreePtr leftSibling=this->findLeftSibling(parent);
        TreePtr rightSibling=this->findRightSibling(parent);
        bool inserted=false;
        // if(!is_null(leftSibling)) {
        //     TreeNode *leftNode = TreeNode::tree_node_factory(leftSibling);
        //     if(!leftNode->is_full()) {
        //         //redistribute to left sibling
        //         auto &leftMapping=leftNode->getMap();
        //         leftMapping.insert(*(this->data_pointers.begin()));
        //         this->data_pointers.erase(this->data_pointers.begin());
        //         leftNode->size++;
        //         this->size--;
        //         inserted=true;
        //     }
        //     leftNode->dump();
        //     delete leftNode;
        // }
        // if(!inserted && !is_null(rightSibling)) {
        //     TreeNode *rightNode = TreeNode::tree_node_factory(rightSibling);
        //     if(!rightNode->is_full()) {
        //         //redistribute to right sibling
        //         auto &rightMapping=rightNode->getMap();
        //         rightMapping.insert(*(prev(this->data_pointers.end())));
        //         this->data_pointers.erase(prev(this->data_pointers.end()));
        //         rightNode->size++;
        //         this->size--;
        //         inserted=true;
        //     }
        //     rightNode->dump();
        //     delete rightNode;
        // }
        if(!inserted) {
            //have to split
            TreeNode* newLeaf = TreeNode::tree_node_factory(LEAF);
            auto &mp=newLeaf->getMap();
            while(this->size>MIN_OCCUPANCY) {
                mp.insert(*prev(this->data_pointers.end()));
                newLeaf->size++;
                this->data_pointers.erase(prev(this->data_pointers.end()));
                this->size--;
            }
            this->next_leaf_ptr=newLeaf->tree_ptr;
            new_leaf=newLeaf->tree_ptr;
            newLeaf->dump();
            delete newLeaf;
        }
    }
    this->dump();
    return new_leaf;
}

//key is deleted from leaf if exists
//TODO: LeafNode::delete_key to be implemented
void LeafNode::delete_key(const Key& key, TreePtr parent) {
    // cout << "LeafNode::delete_key not implemented" << endl;
    if(this->data_pointers.find(key)==this->data_pointers.end()) // key does not exist
        return;
    this->data_pointers.erase(key);
    // cout<<"Leaf, k= "<<key<<endl;
    // for (auto& it:this->data_pointers)
    //     cout<<it.first<<", ";
    // cout<<endl;
    
    this->size--;

    if(this->underflows()) {
        TreePtr leftSibling=this->findLeftSibling(parent);
        if (leftSibling != NULL_PTR){
            TreeNode *leftNode = TreeNode::tree_node_factory(leftSibling);
            auto &leftMapping=leftNode->getMap();
            if(leftNode->size > MIN_OCCUPANCY){
                //take from left sibling
                this->data_pointers.insert(*(leftMapping.rbegin()));
                leftMapping.erase(prev(leftMapping.end())); 
                leftNode->size--;
                this->size++;
            } else {
                // merge with left
                leftMapping.insert(this->data_pointers.begin(),this->data_pointers.end());
                leftNode->size += this->size;
                
                leftNode->set_next_leaf(this->next_leaf_ptr);
                this->size = 0;
                // delete this node
            }
            leftNode->dump();
            delete leftNode;
        } else {
            TreePtr rightSibling=this->findRightSibling(parent);
            TreeNode *rightNode = TreeNode::tree_node_factory(rightSibling);
            auto &rightMapping=rightNode->getMap();
            if(rightNode->size > MIN_OCCUPANCY){
                //take from right sibling
                this->data_pointers.insert(*(rightMapping.begin()));
                rightMapping.erase(rightMapping.begin()); 
                rightNode->size--;
                this->size++;
            } else {
                // merge with right
                rightMapping.insert(this->data_pointers.begin(),this->data_pointers.end());
                rightNode->size += this->size;
                
                rightNode->set_next_leaf(this->next_leaf_ptr);
                this->size = 0;
         
            }
            rightNode->dump();
            delete rightNode;
        }

    }

    this->dump();
}

//runs range query on leaf
void LeafNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for(const auto& data_pointer : this->data_pointers){
        if(data_pointer.first >= min_key && data_pointer.first <= max_key)
            data_pointer.second.write_data(os);
        if(data_pointer.first > max_key)
            return;
    }
    if(!is_null(this->next_leaf_ptr)){
        auto next_leaf_node = new LeafNode(this->next_leaf_ptr);
        next_leaf_node->range(os, min_key, max_key);
        delete next_leaf_node;
    }
}

//exports node - used for grading
void LeafNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for(const auto& data_pointer : this->data_pointers){
        os << data_pointer.first << " ";
    }
    os << endl;
}

//writes leaf as a mermaid chart
void LeafNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    for(const auto& data_pointer: this->data_pointers) {
        chart_node += to_string(data_pointer.first) + " ";
    }
    chart_node += "]";
    os << chart_node << endl;
}

ostream& LeafNode::write(ostream &os) const {
    TreeNode::write(os);
    for(const auto & data_pointer : this->data_pointers){
        if(&os == &cout)
            os << "\n" << data_pointer.first << ": ";
        else
            os << "\n" << data_pointer.first << " ";
        os << data_pointer.second;
    }
    os << endl;
    os << this->next_leaf_ptr << endl;
    return os;
}

istream& LeafNode::read(istream& is){
    TreeNode::read(is);
    this->data_pointers.clear();
    for(int i = 0; i < this->size; i++){
        Key key = DELETE_MARKER;
        RecordPtr record_ptr;
        if(&is == &cin)
            cout << "K: ";
        is >> key;
        if(&is == &cin)
            cout << "P: ";
        is >> record_ptr;
        this->data_pointers.insert(pair<Key,RecordPtr>(key, record_ptr));
    }
    is >> this->next_leaf_ptr;
    return is;
}