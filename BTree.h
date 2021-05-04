#ifndef BTREE_H
#define BTREE_H

#include<iostream>
#include<type_traits>
#include<memory>
#include <experimental/optional>


struct empty_class;

template<typename KEYTYPE, typename VALUETYPE = empty_class> 
class BTree
{
	public:	
		BTree();
		
		BTree(int inputDegree);
		
		void insert(const KEYTYPE& keyItem, const VALUETYPE& valueItem);
		
		void insert(const KEYTYPE& keyItem);
		
		bool isEmpty() const;
		
		int numberOfItems() const;
		
		bool deleteElement(const KEYTYPE& key);
		
		bool contains(const KEYTYPE& key) const;
		
		std::experimental::optional<VALUETYPE> getValue(const KEYTYPE& key) const; 
		
		void printTree(); 
		
		//Copy Constructor
		BTree(const BTree& other) = delete;
		
		//Move Constructor
		BTree(BTree&& other) noexcept;
		
		//Copy Assignemnt
		BTree& operator=(const BTree& other) = delete;
		
		//Move Assignment
		BTree& operator=(BTree&& other) noexcept;
		
		//Destructor
		~BTree();
		
		//Constant iterator for Btree
		struct const_iterator;
		
		//Gives iterator to smallest element in the tree 
		const_iterator cbegin() const;
		
		//Gives iterator to location after biggest element in tree
		const_iterator cend() const;
		
		//Gives iterator to last element in the tree
		const_iterator crbegin() const;
		
		//Gives iterator to location before first element in the tree
		const_iterator crend() const;
		
		
		
	private: 
		struct BTreeImplementation;
		std::shared_ptr<BTreeImplementation> BTreeHandle;
};

#include "BTree.hpp"
#endif //BTREE_H