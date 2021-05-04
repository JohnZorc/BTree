#include <cassert>
#include <vector>
using namespace std; 




/* ====== DATA STRUCTORS FOR THE BTREE ====== */ 
			
struct empty_class {}; 
	
template<typename KEYTYPE, typename VALUETYPE>
struct BTree<KEYTYPE, VALUETYPE>::const_iterator {
	int index;
	//typename std::shared_ptr<BTreeImplementation::node> handle; 
	
	typename BTreeImplementation::node* handle;
	
	const_iterator(typename BTreeImplementation::node* _handle, int _index) {
		handle = _handle;
		index = _index; 
	}
	
	//De-reference
	const KEYTYPE operator* () const {
		
			
		return handle->keys[index];
	}
	
	//pre increment
	const_iterator& operator++ () {
		KEYTYPE currentKey = handle->keys[index];
		typename BTreeImplementation::node* originalHandle = handle;
		
		
		//In leaf with room to move index
		if(handle->leaf && index < handle->currSize - 1) {
			index++;
			return *this;
		}
		//In leaf at last element 
		else if(handle->leaf && index == handle->currSize - 1) {
			//While you have somewhere to go
			while(handle->parent != nullptr) {
				handle = handle->parent.get();
				for(int i = 0; i < handle->currSize; i++) {
					//found key in parent node 
					if(handle->keys[i] >= currentKey) {
						index = i;
						return *this; 
					}
				}
			}
			//Did not find a greater key in the tree
			handle = originalHandle;
			index++;
			return *this;
		}
		//We are not in a leaf
		else if(!handle->leaf) {
			handle = handle->pointers[index + 1].get();
			while (!handle->leaf){
				handle = handle->pointers[0].get();
			}
			// Return the first key of the leaf
			index = 0;
			return *this;
		}
		else 
			return *this;
	}
	
	//post increment
	const_iterator operator++ (int) {
		const_iterator temp = *this;
		++(*this);
		return temp;
	}
	
	//pre decrement
	const_iterator& operator-- () {
		KEYTYPE currentKey = handle->keys[index];
		typename BTreeImplementation::node* originalHandle = handle;
		
		//In leaf with room to move index
		if(handle->leaf && index > 0) {
			index--;
			return *this;
		}
		
		//In leaf at first element 
		else if(handle->leaf && index == 0) {
			//cout << "Parent first element" << handle->parent->keys[0];
			//While you have somewhere to go
			while(handle->parent != nullptr) {
				handle = handle->parent.get();
				for(int i = handle->currSize - 1; i >= 0; i--) {
					//found key in parent node 
					if(handle->keys[i] < currentKey) {
						index = i;
						return *this; 
					}
				}
			}
			//Did not find a lesser key in the tree
			handle = originalHandle;
			index--; 
			return *this;
		}
		
		//We are not in a leaf
		else if(!handle->leaf) {
			handle = handle->pointers[index].get();
			while (!handle->leaf){
				handle = handle->pointers[handle->currSize].get();
			}
			// Return the first key of the leaf
			index = handle->currSize - 1;
			return *this;
		}
		else 
			return *this;
	}
	
	//post decrement
	const_iterator operator-- (int) {
		const_iterator temp = *this;
		--(*this);
		return temp;
	}
	
	bool operator!=(const const_iterator& rhs) {
		if(this->handle->keys[index] == rhs.handle->keys[rhs.index]) {
			return false;
		}
		else  {
			//cout << "They are not equal " << endl;
			return true;
		}
	}
	
	bool operator==(const const_iterator& rhs) {
		if(this->handle->keys[index] == rhs.handle->keys[rhs.index])
			return true;
		else 
			return false;
	}
	
	std::experimental::optional<VALUETYPE> getCurrValue() {
		if(std::is_same<empty_class, VALUETYPE>::value) {
			cout << "\nInvalid get, there is just a key, no value. \nReturning empty object" << endl; 
			return {};
		}
		
		else
			return handle->values[index];
	}
};
	
	
template<typename KEYTYPE, typename VALUETYPE>
struct BTree<KEYTYPE, VALUETYPE>::BTreeImplementation {
	struct node {
		vector<KEYTYPE> keys;
		vector<VALUETYPE> values;
		vector<shared_ptr<node>> pointers;
		std::shared_ptr<node> parent = nullptr; 
		int currSize;
		bool leaf;
		int degree;
		
		node(bool isLeaf, int inputDegree) {
			currSize = 0; 
			leaf = isLeaf; 
			degree = inputDegree;
		}
		
		void printKeys(int level, int child) {
			cout << "Keys Size: " << currSize << " Pointers size: " << pointers.size() << endl;
			for(int i = 0; i < keys.size(); i++) {
				cout << keys[i] << " level: " << level << " Child: " << child << endl;
			}
			level++;
			int childCounter = 0;
			for(int i = 0; i < pointers.size(); i++) {
				pointers[i]->printKeys(level, childCounter);
				childCounter++;
			}
		}
		
		void setLeaf(bool isLeaf) { leaf = isLeaf; }
		
		void addElement(const KEYTYPE& key, const VALUETYPE& value) {
			if(currSize == 0) {
				keys.push_back(key);
				values.push_back(value);
				currSize++;
				return;
			}
			for(int i = 0; i < currSize; i++) {
				if(key < keys[i]) {
					keys.insert(keys.begin() + i, key); 
					values.insert(values.begin() + i, value); 
					currSize++;
					return;
				}
			}
			keys.push_back(key); 
			values.push_back(value); 
			currSize++;
		}
		
		void addElement(const KEYTYPE& key) {
			if(currSize == 0) {
				keys.push_back(key);
				currSize++;
				return;
			}
			for(int i = 0; i < currSize; i++) {
				if(key < keys[i]) {
					keys.insert(keys.begin() + i, key); 
					currSize++;
					return;
				}
			}
			keys.push_back(key); 
			currSize++;
		}
		
		int findGreaterOrEqual(const KEYTYPE& key) const {
			int index = 0; 
			while(index < currSize && keys[index] < key) {
				index++; 
			}
			return index;
		}
		
		KEYTYPE getPredKey(int index) {
			// Keep moving to the right most node until we reach a leaf
			std::shared_ptr<node> currNode = pointers[index];
			while (!currNode->leaf)
				currNode = currNode->pointers[currNode->currSize];
		  
			// Return the last key of the leaf
			return currNode->keys[currNode->currSize -1];
		}
		
		VALUETYPE getPredValue(int index) {
			// Keep moving to the right most node until we reach a leaf
			std::shared_ptr<node> currNode = pointers[index];
			while (!currNode->leaf)
				currNode = currNode->pointers[currNode->currSize];
		  
			// Return the last key of the leaf
			return currNode->values[currNode->currSize -1];
		}
		
		KEYTYPE getSuccKey(int index) {
			// Keep moving to the right most node until we reach a leaf
			std::shared_ptr<node> currNode = pointers[index + 1];
			while (!currNode->leaf)
				currNode = currNode->pointers[0];
		  
			// Return the first key of the leaf
			return currNode->keys[0];
		}
		
		VALUETYPE getSuccValue(int index) {
			// Keep moving to the right most node until we reach a leaf
			std::shared_ptr<node> currNode = pointers[index + 1];
			while (!currNode->leaf)
				currNode = currNode->pointers[0];
		  
			// Return the last key of the leaf
			return currNode->values[0];
		}
		
		void merge(int index) {
			std::shared_ptr<node> child = pointers[index];
			std::shared_ptr<node> sibling = pointers[index + 1]; 
			
			
			// Pulling a key from the current node and inserting it into (degree-1)th
			// position of pointers[index]
			child->keys.push_back(keys[index]);
			if(!(std::is_same<empty_class, VALUETYPE>::value))
				child->values.push_back(values[index]);
			
			// pushing_back the keys from pointers[index + 1] to pointers[index]
			for (int i=0; i<sibling->currSize; ++i) {
				child->keys.push_back(sibling->keys[i]);
				if(!(std::is_same<empty_class, VALUETYPE>::value))
					child->values.push_back(sibling->values[i]);
			}
			
			 // Copying the child pointers from pointers[index + 1] to pointed[index]
			if (!child->leaf)
			{
				for(int i=0; i<=sibling->currSize; ++i)
					child->pointers.push_back(std::move(sibling->pointers[i]));
			}
			
			// Moving all keys after index in the current node one step before -
			// to fill the gap created by moving keys[idx] to C[idx]
			keys.erase(keys.begin() + index);
			if(!(std::is_same<empty_class, VALUETYPE>::value))
				values.erase(values.begin() + index);
			
			// Moving the child pointers after (index + 1) in the current node one
			// step before
			pointers.erase(pointers.begin() + (index + 1));
			
			// Updating the key count of child and the current node
			child->currSize = (degree * 2) - 1;
			currSize--;
		}
		
		void borrowFromNext(int index) {
			std::shared_ptr<node> child = pointers[index];
			std::shared_ptr<node> sibling = pointers[index + 1];
			
			
			// The last key from pointers[index-1] goes up to the parent and key[index-1]
			// from parent is inserted as the first key in pointers[index]. Thus, the sibling 
			// loses one key and child gains one key
			
			// Setting child's last key equal to keys[index] from the current node
			child->keys.push_back(keys[index]);
			
			if(!(std::is_same<empty_class, VALUETYPE>::value))
				child->values.push_back(values[index]);
		  
			// Moving sibling's first child as pointer[indexs]'s last child
			if(!child->leaf) {
				child->pointers.push_back(std::move(sibling->pointers[0]));
				child->pointers[child->currSize + 1]->parent = child;
				sibling->pointers.erase(sibling->pointers.begin());
			}
			
			
			// Moving the key from the sibling to the parent
			// This reduces the number of keys in the sibling
			keys[index] = sibling->keys[0];
			sibling->keys.erase(sibling->keys.begin());
			
			if(!(std::is_same<empty_class, VALUETYPE>::value)) {
				values[index] = sibling->values[0];
				sibling->values.erase(sibling->values.begin());
			}
		  
			child->currSize += 1;
			sibling->currSize -= 1;
			
		}
		
		void insertNonFull(const KEYTYPE& key) {
		//Right most element
		int i = currSize - 1;
		
		//If non full leaf just add
		if(leaf == true) {
			addElement(key);
		}
		
		//Not a leaf 
		else {
			//sets i = index of child to enter
			while(i >=0 && keys[i] > key)
				i--;
			
			//Is child full 
			if(pointers[i+1]->currSize == (2 * degree - 1)) {
				split_child(i + 1);
				
				if (key > keys[i +1])
					i++;
			}
			
			pointers[i + 1]->insertNonFull(key);
			
		}
	}
	
		void insertNonFull(const KEYTYPE& key, const VALUETYPE& value) {
			//Right most element
			int i = currSize - 1;
			
			//If non full leaf just add
			if(leaf == true) {
				addElement(key, value);
			}
			
			//Not a leaf 
			else {
				//sets i = index of child to enter
				while(i >=0 && keys[i] > key)
					i--;
				
				//Is child full 
				if(pointers[i+1]->currSize == (2 * degree - 1)) {
					split_child(i + 1);
					
					if (key > keys[i +1])
						i++;
				}
				
				pointers[i + 1]->insertNonFull(key, value);
				
			}
		}
	
		void split_child(int index) {
		std::shared_ptr<node> child = pointers[index];
		
		//Make new sibling node
		std::shared_ptr<node> siblingNode = std::make_shared<node>(child->leaf, degree);
		siblingNode->parent = child->parent; 
		
		//copy the last degree - 1 keys from child to sibling
		for(int j = 0; j < degree-1; j++) {
			siblingNode->keys.push_back(child->keys[j+degree]);
			if(!(std::is_same<empty_class, VALUETYPE>::value)) {
				siblingNode->values.push_back(child->values[j+degree]);
			}
			siblingNode->currSize++;
		}

		//Move the pointers
		if(child->leaf == false)
		{
			for (int j = 0; j < degree; j++) {
				siblingNode->pointers.push_back(std::move(child->pointers[degree+j]));
				siblingNode->pointers[j]->parent = siblingNode;
			}
		}
		
		// Reduce the number of keys in child
		child->currSize = degree - 1;
		
		pointers.insert(pointers.begin() + index + 1, std::move(siblingNode));
		
		
		// Copy the middle key and value of child node to this node
		keys.insert(keys.begin() + index, child->keys[degree-1]);
		if(!(std::is_same<empty_class, VALUETYPE>::value))
			values.insert(values.begin() + index, child->values[degree-1]);
		
		// Delete keys and pointers and values from the child node
		for(int j = 0; j < degree; j++) {
			child->keys.pop_back(); 
			if(!(std::is_same<empty_class, VALUETYPE>::value)) {
				child->values.pop_back();
			}
		}
		if(child->leaf == false)
		{
			for (int j = 0; j < degree; j++)
				child->pointers.pop_back();
		}
		
		// Increment count of keys in this node
		currSize = currSize + 1;
	}
	
		// A function to take a key from pointers[index-1] and insert it
		// into pointers[index]
		void borrowFromPrev(int index) {
			std::shared_ptr<node> child = pointers[index];
			std::shared_ptr<node> sibling = pointers[index-1];
			
			// The last key from pointers[index-1] goes up to the parent and key[index-1]
			// from parent is inserted as the first key in pointers[index]. Thus, the sibling 
			// loses one key and child gains one key
			
			// Setting child's first key equal to keys[index-1] from the current node
			child->keys.insert(child->keys.begin(), keys[index-1]);
			if(!(std::is_same<empty_class, VALUETYPE>::value))
				child->values.insert(child->values.begin(), values[index-1]);
		  
			// Moving sibling's last child as pointer[indexs]'s first child
			if(!child->leaf) {
				child->pointers.insert(child->pointers.begin(), std::move(sibling->pointers[sibling->currSize]));
				sibling->pointers.pop_back();
				child->pointers[0]->parent = child;
			}
			// Moving the key from the sibling to the parent
			// This reduces the number of keys in the sibling
			keys[index - 1] = sibling->keys[sibling->currSize - 1];
			sibling->keys.pop_back();
			if(!(std::is_same<empty_class, VALUETYPE>::value)) {
				values[index - 1] = sibling->values[sibling->currSize - 1];
				sibling->values.pop_back();
			}
			child->currSize++;
			sibling->currSize--;
		} 
		
		//A function that fills the child at pointer[index] so you may move into it
		void fill(int index) {
			// If the previous child(pointers[index-1]) has more than degree -1 keys, take a key
			// from that child
			if (index!=0 && pointers[index-1]->currSize >= degree)
				borrowFromPrev(index);
		  
			// If the next child(pointers[index+1]) has more than degree-1 keys, take a key
			// from that child
			else if (index != currSize && pointers[index+1]->currSize >= degree)
				borrowFromNext(index);
		  
			// Merge pointers[index] with its sibling
			// If pointers[index] is the last child, merge it with with its previous sibling
			// Otherwise merge it with its next sibling
			else
			{
				if (index != currSize) {
					merge(index);
				}
				else {
					merge(index-1);
				}
			}
		}
		
		bool removeFromLeaf(int index){
			keys.erase(keys.begin() + index);
			if(!(std::is_same<empty_class, VALUETYPE>::value))
				values.erase(values.begin() + index);
			currSize--; 
			return true; 
		}
		
		bool removeFromNonLeaf(int index) {
			KEYTYPE key = keys[index]; 
		
			// If the child that precedes key (pointers[index]) has at least degree keys,
			// find the predecessor 'pred' of key in the subtree rooted at
			// pointers[index]. Replace key by pred. Recursively delete pred
			// in pointers[index]
			/*for(int i = 0; i < pointers.size(); i++) {
				if (pointers[i])
					cout << "not null" << endl;
				else 
					cout << "NUll" << endl;
			}
			cout << "index: " << index << " Pointers size: " << pointers.size() << endl; // pointers[index]->leaf <<
			cout << "keys size" << keys.size() << endl;
			for(int i = 0; i < pointers[index]->keys.size(); i++) 
				cout << "Keys at [" << i << "] " << pointers[index]->keys[i] << endl;
			*/
			if (pointers[index]->currSize >= degree)
			{
				KEYTYPE predKey = getPredKey(index);
				keys[index] = predKey;
				
				if(!(std::is_same<empty_class, VALUETYPE>::value)) {
					VALUETYPE predVal = getPredValue(index);
					values[index] = predVal;
				}
				return pointers[index]->remove(predKey);
			}
			
			// If the child pointers[index] has less that degree keys, examine pointers[index +1].
			// If pointers[index + 1] has atleast degree keys, find the successor 'succ' of key in
			// the subtree rooted at pointers[index + 1]
			// Replace key by succ
			// Recursively delete succ in pointers[index + 1]
			else if (pointers[index + 1]->currSize >= degree)
			{
				//cout << "Removing from now leaf1" << endl;
				KEYTYPE succKey = getSuccKey(index);
				keys[index] = succKey;
				
				if(!(std::is_same<empty_class, VALUETYPE>::value)) {
					VALUETYPE succVal = getSuccValue(index);
					values[index] = succVal;
				}
				return pointers[index +1]->remove(succKey);
			}
			
			// If both pointers[index] and pointers[index + 1] have less than degree keys,
			// merge key and all of pointers[index + 1] into pointers[index]
			// Now pointers[index] contains 2t-1 keys
			// Free pointers[index +1] and recursively delete key from pointers[index]
			else
			{
				//cout << "Removing from now leaf2" << endl;
				//cout << "Merging" << endl;
				merge(index);
				return pointers[index]->remove(key);
			}			
		}
		
		//Removes key from a subtree rooted with this node
		bool remove(const KEYTYPE& key){
			
			int index = findGreaterOrEqual(key); 
			
			//The key is in this node 
			if(index < currSize && keys[index] == key ) {
				if(leaf) {
					return removeFromLeaf(index);
				}
				else {
					return removeFromNonLeaf(index); 
				}
			}
			
			//The key is not in this node
			else {
				if(leaf) {
					return false;
				}
				bool flag ((index == currSize)? true : false);
				
				if (pointers[index]->currSize < degree) {
					fill(index);
				}
				//This can only happen if during the fill phase the last child was merged with
				//its relative, shrinking the numbers of children. 
				if (flag && index > currSize) {
					
					return pointers[index-1]->remove(key);
				}
				else {
					return pointers[index]->remove(key);
				}
				
			}
		}
	};
	
	int currNumElements;
	int degree;
	int height;
	std::shared_ptr<node> root;
	
	void Print() {
		if(root)
			root->printKeys(0, 0);
		else 
			cout << "THE TREE IS EMPTY" << endl;
	}
	
	void insert2(const KEYTYPE& key, const VALUETYPE& value) { 
		if(contains(key)){
			return;
		}
		//If root is empty
		if(root == NULL) {
			root = std::make_shared<node>(true, degree);
			root->addElement(key, value);
			root->currSize = 1;
		}
		//The tree is not empty
		else  {
			//Root is full, tree grows
			if(root->currSize == 2 * degree - 1) {
				std::shared_ptr<node> newNode = std::make_shared<node>(false, degree);
				newNode->pointers.push_back(std::move(root));
				newNode->pointers[0]->parent = newNode;
				newNode->split_child(0);
				height++;
				int i = 0;
				if(newNode->keys[0] < key)
					i++;
				newNode->pointers[i]->insertNonFull(key, value);
				
				root = std::move(newNode);
			}
			//Root is not full
			else  {
				root->insertNonFull(key, value);
			}
		}
		currNumElements++;
	}
	
	void insert1(const KEYTYPE& key) {
		if(contains(key)){
			return;
		}
		//If root is empty
		if(root == NULL) {
			root = std::make_shared<node>(true, degree);
			root->addElement(key);
			root->currSize = 1;
		}
		
		//The tree is not empty
		else  {
			//Root is full, tree grows
			if(root->currSize == 2 * degree - 1) {
				std::shared_ptr<node> newNode = std::make_shared<node>(false, degree);
				newNode->pointers.push_back(std::move(root));
				newNode->pointers[0]->parent = newNode;
				newNode->split_child(0);
				height++;
				int i = 0;
				if(newNode->keys[0] < key)
					i++;
				newNode->pointers[i]->insertNonFull(key);
				
				root = std::move(newNode);
			}
			//Root is not full
			else  {
				root->insertNonFull(key);
			}
		}
		currNumElements++;
	}
	
	bool contains(const KEYTYPE& keyItem) {
		//If empty tree
		if(!root)
			return false;
		
		std::shared_ptr<node> currNode = root;
		while(!currNode->leaf) {
			bool greaterThanAll = true;
			for(int i = 0; i < currNode->currSize; i++) {
				if(keyItem < currNode->keys[i]) {
					currNode = currNode->pointers[i];
					greaterThanAll = false;
					break;
				}
				else if(keyItem == currNode->keys[i]) {
					//cout << "Hello" << endl;
					return true;
				}
			}
			
			//KeyValue was greater than all the keys in this node
			if(greaterThanAll) {
				int index = currNode->currSize;
				currNode = currNode->pointers[index];
			}
		}
		for(int i = 0; i < currNode->currSize; i++) {
			if(keyItem == currNode->keys[i]) 
				return true;
		}
		return false;
	}
	
	std::experimental::optional<VALUETYPE> getValue(const KEYTYPE& keyItem) {
		std::shared_ptr<node> currNode = root;
		while(!currNode->leaf) {
			bool greaterThanAll = true;
			for(int i = 0; i < currNode->currSize; i++) {
				if(keyItem < currNode->keys[i]) {
					currNode = currNode->pointers[i];
					greaterThanAll = false;
					break;
				}
				else if(keyItem == currNode->keys[i]) {
					return currNode->values[i];
				}
			}
			
			//KeyValue was greater than all the keys in this node
			if(greaterThanAll) {
				int index = currNode->currSize;
				currNode = currNode->pointers[index];
			}
		}
		for(int i = 0; i < currNode->currSize; i++) {
			if(keyItem == currNode->keys[i]) {
				return currNode->values[i];
			}
		}
		return {};
	}
	
	bool deleteElement(const KEYTYPE& key) {
		bool returnVal = true;
		if(currNumElements == 0) {
			std::cout << "\nBTree is empty. Deletion is invalid." << endl;
			return false;
		}
		else {
			returnVal = root->remove(key);
			if(root->currSize == 0){
				//cout << "Roots current size is 0 yay!" << endl;
				if(root->leaf) {
					//cout << "Root is a leaf" << endl;
					root.reset();
				}
				else {
					std::shared_ptr<node> temp = std::move(root->pointers[0]);
					root = std::move(temp); 
				}
			}
		}
		return returnVal;
	}
};



/* ====== CONSTRUCTORS ====== */ 

template<typename KEYTYPE, typename VALUETYPE> 
BTree<KEYTYPE, VALUETYPE>::BTree(int inputDegree) {
	
	BTreeHandle = std::make_shared<BTreeImplementation>();
	BTreeHandle->currNumElements = 0;
	BTreeHandle->degree = inputDegree;
	BTreeHandle->height = 0;
	BTreeHandle->root = NULL;
}

template<typename KEYTYPE, typename VALUETYPE> 
BTree<KEYTYPE, VALUETYPE>::BTree() {
	
	BTreeHandle = std::make_shared<BTreeImplementation>();
	BTreeHandle->currNumElements = 0;
	BTreeHandle->degree = 50;
	BTreeHandle->height = 0;
	BTreeHandle->root = NULL;
}




/* ====== INSERT IMPLEMENTATIONS ====== */ 

template<typename KEYTYPE, typename VALUETYPE> 
void BTree<KEYTYPE, VALUETYPE>::insert(const KEYTYPE& keyItem, const VALUETYPE& valueItem) {
	static_assert(!(std::is_same<empty_class, VALUETYPE>::value)); 
	BTreeHandle->insert2(keyItem, valueItem);
}

template<typename KEYTYPE, typename VALUETYPE> 
void BTree<KEYTYPE, VALUETYPE>::insert(const KEYTYPE& keyItem) {
	static_assert((std::is_same<empty_class, VALUETYPE>::value));
	BTreeHandle->insert1(keyItem);
}




/* ====== HELPER METHODS ======*/

template<typename KEYTYPE, typename VALUETYPE> 
bool BTree<KEYTYPE, VALUETYPE>::isEmpty() const{
	if(BTreeHandle->currNumElements == 0)
		return true;
	else 
		return false;
}

template<typename KEYTYPE, typename VALUETYPE> 
void BTree<KEYTYPE, VALUETYPE>::printTree() {
	BTreeHandle->Print();
}
template<typename KEYTYPE, typename VALUETYPE> 
int BTree<KEYTYPE, VALUETYPE>::numberOfItems() const{
	return BTreeHandle->currNumElements;
}

template<typename KEYTYPE, typename VALUETYPE> 
bool BTree<KEYTYPE, VALUETYPE>::contains(const KEYTYPE& key) const {
	return BTreeHandle->contains(key);
}

template<typename KEYTYPE, typename VALUETYPE> 
std::experimental::optional<VALUETYPE> BTree<KEYTYPE, VALUETYPE>::getValue(const KEYTYPE& key) const{

	if(std::is_same<empty_class, VALUETYPE>::value) {
		cout << "\nInvalid get, there is just a key, no value. \nReturning empty object" << endl; 
		return {};
	}
	
	return BTreeHandle->getValue(key);
	
	return {};
}



/* ====== DELETION ====== */

template<typename KEYTYPE, typename VALUETYPE> 
bool BTree<KEYTYPE, VALUETYPE>::deleteElement(const KEYTYPE& key){
	if(BTreeHandle->deleteElement(key)) {
		BTreeHandle->currNumElements--;
		return true;
	}
	else 
		return false; 
}


/* ====== The Rule of 5 methods ======*/ 

//Destructor
template<typename KEYTYPE, typename VALUETYPE> 
BTree<KEYTYPE, VALUETYPE>::~BTree() {
	std::cout << "Calling the destructor" << std::endl; 
}

// Move Constructor
template<typename KEYTYPE, typename VALUETYPE> 
BTree<KEYTYPE, VALUETYPE>::BTree(BTree&& other) noexcept {
	this->BTreeHandle = std::move(other.BTreeHandle);
}

// Move Assignment
template<typename KEYTYPE, typename VALUETYPE> 
BTree<KEYTYPE, VALUETYPE>& BTree<KEYTYPE, VALUETYPE>::operator=(BTree&& other) noexcept {
	this->BTreeHandle = std::move(other.BTreeHandle);
	return *this;
}



/* ====== ITERATOR METHODS ===== */
template<typename KEYTYPE, typename VALUETYPE> 
typename BTree<KEYTYPE, VALUETYPE>::const_iterator BTree<KEYTYPE, VALUETYPE>::cbegin() const{
	typename  BTreeImplementation::node* currNode = BTreeHandle->root.get(); 
	
	//Smallest element will be root[0] if the height is 1
	if(BTreeHandle->root->leaf) {
		const_iterator newIterator = const_iterator(currNode, 0);
		return newIterator;
	}
	//Root is not leaf
	else {
		while(!currNode->leaf)
			currNode = currNode->pointers[0].get();
		const_iterator newIterator = const_iterator(currNode, 0);
		return newIterator;
	}
}


template<typename KEYTYPE, typename VALUETYPE> 
typename BTree<KEYTYPE, VALUETYPE>::const_iterator BTree<KEYTYPE, VALUETYPE>::cend() const{
	typename  BTreeImplementation::node* currNode = BTreeHandle->root.get(); 
	
	//Gives the location AFTER the last element
	if(BTreeHandle->root->leaf) {
		const_iterator newIterator = const_iterator(currNode, currNode->currSize);
		return newIterator;
	}
	//Root is not leaf
	else {
		while(!currNode->leaf)
			currNode = currNode->pointers[currNode->currSize].get();
		const_iterator newIterator = const_iterator(currNode, currNode->currSize);
		return newIterator;
	}
}

template<typename KEYTYPE, typename VALUETYPE> 
typename BTree<KEYTYPE, VALUETYPE>::const_iterator BTree<KEYTYPE, VALUETYPE>::crbegin() const{
	typename  BTreeImplementation::node* currNode = BTreeHandle->root.get(); 
	
	//Gives the location of the last element
	if(BTreeHandle->root->leaf) {
		const_iterator newIterator = const_iterator(currNode, currNode->currSize - 1);
		return newIterator;
	}
	//Root is not leaf
	else {
		while(!currNode->leaf)
			currNode = currNode->pointers[currNode->currSize].get();
		const_iterator newIterator = const_iterator(currNode, currNode->currSize - 1);
		return newIterator;
	}
}


template<typename KEYTYPE, typename VALUETYPE> 
typename BTree<KEYTYPE, VALUETYPE>::const_iterator BTree<KEYTYPE, VALUETYPE>::crend() const{
	typename  BTreeImplementation::node* currNode = BTreeHandle->root.get(); 
	
	//Smallest element will be root[0] if the height is 1
	if(BTreeHandle->root->leaf) {
		const_iterator newIterator = const_iterator(currNode, -1);
		return newIterator;
	}
	//Root is not leaf
	else {
		while(!currNode->leaf)
			currNode = currNode->pointers[0].get();
		const_iterator newIterator = const_iterator(currNode, -1);
		return newIterator;
	}
}









