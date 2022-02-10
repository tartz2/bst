/*bstt.h*/
// Name: Thomas Artz
// 
// Threaded binary search tree
//

#pragma once

#include <iostream>

using namespace std;

template<typename KeyT, typename ValueT>

class bstt
{
private:
	struct NODE
	{
		KeyT   Key;
		ValueT Value;
		NODE*  Left;
		NODE*  Right;
		bool   isThreaded;
	};

	NODE* Root;  // pointer to root node of tree (nullptr if empty)
	int   Size;  // # of nodes in the tree (0 if empty)
	NODE* ogRoot; // Allows "Root" to be moved while this one holds its original place
	bool hasBegun; // Checks whether or not the tree has called begin()
	
	// insertCopy()
	// 
	// Called when making a copy of a tree/node. It is called recursively
	// so that the whole tree ends up being copied to the new one.
	// If the node it's trying to insert is a nullptr, it doesnt insert and
	// returns
	// 
	void insertCopy(NODE* orig)
	{
		if(orig == nullptr)
			return;
		insert(orig->Key, orig->Value);
		
		insertCopy(orig->Left);
		if(!orig->isThreaded)
			insertCopy(orig->Right);
		return;
	}
	
	// inOrder()
	// 
	// called by the dump() function. This is a recursive function that 
	// prints the tree in order from smallest to largest value. If it is threaded,
	// it prints which node it is threaded to.
	// 
	void inOrder(NODE* cur, ostream& output) const
	{
		if(cur != nullptr)
		{
		
			inOrder(cur->Left, output);
			if(cur->isThreaded)
				output << "(" << cur->Key << "," << cur->Value << "," << cur->Right->Key << ")"<< endl;
			else
				output << "(" << cur->Key << "," << cur->Value << ")" << endl;
			if(cur->isThreaded == false)
				inOrder(cur->Right, output);
			return;
		}
		return;
	}
	
	// clearTree()
	// 
	// function to clear the entire tree by recursively going from left to right
	// through the tree and then deleting the node before returning
	//  
	void clearTree(NODE* node)
	{
		if(node)
		{
			clearTree(node->Left);
			if(node->isThreaded == false)
				clearTree(node->Right);
			delete node;
		}
		
		return;
	}
	
public:
  //
  // default constructor:
  //
  // Creates an empty tree.
  //
	bstt()
	{
		ogRoot = nullptr;
		Root = nullptr;
		Size = 0;
		hasBegun = false;
	}

  //
  // copy constructor
  //
	bstt(const bstt& other)
	{
		ogRoot = nullptr;
		Root = nullptr;
		insertCopy(other.Root);
		Size = other.Size;
		hasBegun = other.hasBegun;
	}

	//
  // destructor:
  //
  // Called automatically by system when tree is about to be destroyed;
  // this is our last chance to free any resources / memory used by
  // this tree.
  //
	virtual ~bstt()
	{
		clearTree(ogRoot);
		Size = 0;
	}
	
	//
	// clear:
	//
	// Performs the same action as the destructors except it also sets the Root
	// equal to nullptr
	//
	void clear() 
	{
		// calls clearTree and then resetting Root and ogRoot to nullptr
		clearTree(ogRoot);
		Root = nullptr;
		ogRoot = nullptr;
		Size = 0;
	}


  //
  // operator=
  //
  // Clears "this" tree and then makes a copy of the "other" tree.
  //
	bstt& operator=(const bstt& other)
	{
		this->clear();
		insertCopy(other.Root);
		Size = other.Size;
		return *this;
	}
	
  // 
  // size:
  //
  // Returns the # of nodes in the tree, 0 if empty.
  //
  // Time complexity:  O(1) 
  //
	int size() const
	{
		return Size;
	}

  // 
  // search:
  //
  // Searches the tree for the given key, returning true if found
  // and false if not.  If the key is found, the corresponding value
  // is returned via the reference parameter.
  //
  // Time complexity:  O(lgN) on average
  //
	bool search(KeyT key, ValueT& value) const
	{
     //
     // TODO:
     //
		NODE* cur = Root;
		if(cur == nullptr)
		{
			return false;
		}
		
		
		while(cur != nullptr) // while cur is still a node
		{
			if(key == cur->Key) // checks if the key is found
			{
				value = cur->Value;
				return true;     // returns true if so and updates value
			} 
			else if(key < cur->Key)
			{
				cur = cur->Left;    // if cur->Key is bigger, it moves left (to a smaller)
			}                     // value and then runs the loop again
			else 
			{
				if(cur->isThreaded){ 
					return false;    // if the node is threaded but the key its trying to find is bigger
				}                  // it returns false because we dont need to go back up the tree. 
				else  
				{
					if(key > cur->Key)  // if its not threaded and the key is bigger than the node's key
						cur = cur->Right; // it moves right to a node with a greater value
				}
			}
		}

		return false; // if it breaks out of the loop it didn't find it and cur == nullptr
	}

  //
  // insert
  //
  // Inserts the given key into the tree; if the key has already been insert then
  // the function returns without changing the tree.
  //
  // Time complexity:  O(lgN) on average
  //
	void insert(KeyT key, ValueT value)
	{
		NODE* prev = nullptr;
		NODE* cur = ogRoot;
		bool goRight = false;
		bool goLeft = false;
	
		
		while (cur != nullptr)
		{  
			if (key == cur->Key)  // already in tree
				return;
				
			else if (key < cur->Key)  // search left:
			{
				prev = cur;
				cur = cur->Left;
				if(cur == nullptr)  // if cur is nullptr after moving left, it shouldnt be trying
					goLeft = true;    // to insert on the left side of this. it just fell out
			}
			else  //search right
			{
				if(cur->isThreaded) // if it hits this point and it's threaded and the key is greater
				{                   // than the current node, it wont go back up the tree. it will
					prev = cur;       // just insert on the right and make it unthreaded
					cur = nullptr;
					goRight = true;
					break;
				}
				else 
				{
					
					prev = cur;
					cur = cur->Right;
					if(cur == nullptr) // if cur is nullptr after moving right, it shouldnt be trying
						goRight = true;  // to insert on the right of this. it just fell out here
				}
					
			}
		}
		
		// creating new node to insert in the tree...
		NODE* newNode = new NODE();

		newNode->Key = key;
		newNode->Value = value;
		newNode->Left = nullptr;
		newNode->Right = nullptr;      
		
		// Determining if its a new tree,
		if (prev == nullptr)
		{
		   Root = newNode;
			 ogRoot = Root;
		}
		
		// or a new node thats to the left of where it fell out,
		if(goLeft)
		{
			newNode->Right = prev;
			prev->Left = newNode;
			newNode->isThreaded = true;
		}
		
		// or a node to the right of where it fell out.
		if(goRight)
		{
			newNode->Right = prev->Right;
			prev->Right = newNode;
			prev->isThreaded = false;     // dethreaded prev if it is going to be inserted on the right of prev
			
			if(newNode->Right != nullptr) // if prev was pointing at something, it uses its right to point there
				newNode->isThreaded = true;
			else
				newNode->isThreaded = false; // and if it wasn't, then it just makes 
		}                                // it null (could be on the right of the tree)

		Size++;
	}

  //
  // []
  //
  // Returns the value for the given key; if the key is not found,
  // the default value ValueT{} is returned.
  //
  // Time complexity:  O(lgN) on average
  //
	ValueT operator[](KeyT key) const
	{
    NODE* cur = ogRoot;
		while(cur != nullptr) // performs a search for the node with a key
		{
			if(key == cur->Key)
				return cur->Value;
				
			if(key < cur->Key)
				cur = cur->Left;
				
			else
			{
				if(cur->isThreaded)  // if the key is greater than the nodes key and its 
					return ValueT{};   // threaded, then the (key,value) node D.N.E.
				else      
					cur = cur->Right;
			}
		}
		

		return ValueT{ }; // if cur went to nullptr, it returns default
	}

  //
  // ()
  //
  // Finds the key in the tree, and returns the key to the "right".
  // If the right is threaded, this will be the next inorder key.
  // if the right is not threaded, it will be the key of whatever
  // node is immediately to the right.
  //
  // If no such key exists, or there is no key to the "right", the
  // default key value KeyT{} is returned.
  //
  // Time complexity:  O(lgN) on average
  //
	KeyT operator()(KeyT key) const
	{
    //
    // TODO
    //

    NODE* cur = ogRoot;
		
		if(ogRoot == nullptr) // if the real root d.n.e, there wont be one
		{                     // to the right
			return KeyT{};
		}
		
		cur = ogRoot;   // starts from the top of the tree

		while(cur != nullptr){
			if(key == cur->Key)
			{
				if(cur->Right != nullptr)      // found, return the value to the right
					return cur->Right->Key;   
				else
					return KeyT{};            // found, but the right node is null
			} 
			else if(key < cur->Key)
			{
				cur = cur->Left;  
			}
			else 
			{
				if(cur->isThreaded){
					return KeyT{};        // if it didnt find it, and its threaded
				}                       // dont go back, return default Key{}T
				else 
				{
					cur = cur->Right;
				}
			}
		}

			return KeyT{};       // if cur went to nullptr, return default
	}

  //
  // begin
  //
  // Resets internal state for an inorder traversal.  After the 
  // call to begin(), the internal state denotes the first inorder
  // key; this ensure that first call to next() function returns
  // the first inorder key.
  //
  // Space complexity: O(1)
  // Time complexity:  O(lgN) on average
  //
  // Example usage:
  //    tree.begin();
  //    while (tree.next(key))
  //      cout << key << endl;
  //
	void begin()
	{
		if(ogRoot == nullptr){
			return;
		}
		
		Root = ogRoot; // moving root back to the original spot
		
		while(Root->Left != nullptr) // and then moving all the way left
		{                            // to first inorder node
			Root = Root->Left;
		}
		
		hasBegun = true; // lets the tree know it has used begin() so it may use next()
    return;
	}

  //
  // next
  //
  // Uses the internal state to return the next inorder key, and 
  // then advances the internal state in anticipation of future
  // calls.  If a key is in fact returned (via the reference 
  // parameter), true is also returned.
  //
  // False is returned when the internal state has reached null,
  // meaning no more keys are available.  This is the end of the
  // inorder traversal.
  //
  // Space complexity: O(1)
  // Time complexity:  O(lgN) on average
  //
  // Example usage:
  //    tree.begin();
  //    while (tree.next(key))
  //      cout << key << endl;
  //
	bool next(KeyT& key)
	{
    //
    // TODO
    //
    if(Root == nullptr || hasBegun == false) // checks that it can use the function
		{
			return false;
		}
	
		key = Root->Key;         // updates key to current Root's key
		Root = Root->Right;      // moves the Root to the right no matter what
		
		if(Root != nullptr){                                     // now it checks if it can move left
			while(Root->Left != nullptr && Root->Left->Key > key)  // so it gets the next inorder value 
			{                                                      // and then it will move if the left values
				Root = Root->Left;                                   // are greater than the key it updated
			}
		}
		return true;
		
	}

  //
  // dump
  // 
  // Dumps the contents of the tree to the output stream, using a
  // recursive inorder traversal.
  //
	void dump(ostream& output) const
	{
		output << "**************************************************" << endl;
		output << "********************* BSTT ***********************" << endl;

		output << "** size: " << this->size() << endl;

    //
    // inorder traversal, with one output per line: either 
    // (key,value) or (key,value,THREAD)
    //
    // (key,value) if the node is not threaded OR thread==nullptr
    // (key,value,THREAD) if the node is threaded and THREAD denotes the next inorder key
    //

    //
    // TODO
    //
    
		
		NODE* cur = ogRoot;
		inOrder(cur, output);     // calls the recursive function to print
		
		output << "**************************************************" << endl;
	}
	
	
	
};

