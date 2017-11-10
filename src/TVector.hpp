////////////////////////////////////////////////////////////
//
// https://github.com/assematt | assenza.matteo@gmail.com
// Copyright (C) - 2017 - Assenza Matteo
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files(the "Software"), to deal in the Software without restriction, 
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or substantial 
// portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Inspired by Vittorio Romeo's https://github.com/SuperV1234/Tutorials/tree/master/Iterators
//
////////////////////////////////////////////////////////////

#pragma once

#include <cassert>
#include <vector>

template <class Type>
class TVector
{

private:

	struct Atom;
	struct Mark;
	struct Iterator;

public:

	// Type aliases
	using Data = std::aligned_storage_t<sizeof(Type), alignof(Type)>;
	using Reference = Type&;
	using CReference = const Type&;
	using Pointer = Type*;
	using Position = unsigned int;
	using Size = unsigned int;
	using CIterator = const Iterator;

private:

	// The "Atom" creates a link between the data in the array and the mark
	struct Atom
	{
		// Constructors
		Atom() :
			mMarkPos(-1),
			mDataPos(-1)
		{
		}
		Atom(const Atom& Copy) :
			mMarkPos(Copy.mMarkPos),
			mDataPos(Copy.mDataPos)
		{
		}
		Atom(Atom&& Move) :
			mMarkPos(Move.mMarkPos),
			mDataPos(Move.mDataPos)
		{
			Move.mMarkPos = -1;
			Move.mDataPos = -1;
		}

		// Construct a mark with a specified Data Position and Mark Position
		Atom(const Position& DataPos, const Position& MarkPos) :
			mMarkPos(MarkPos),
			mDataPos(DataPos)
		{
		}

		// Assignment operator
		Atom& operator=(const Atom& Copy)
		{
			mMarkPos = Copy.mMarkPos;
			mDataPos = Copy.mDataPos;

			return *this;
		}
		Atom& operator=(Atom&& Move)
		{
			mMarkPos = Move.mMarkPos;
			mDataPos = Move.mDataPos;

			Move.mMarkPos = -1;
			Move.mDataPos = -1;

			return *this;
		}

		Position mDataPos;
		Position mMarkPos;
	};

	// The "Mark" creates a link between the iterator and the atom
	struct Mark
	{
		// Alias for the position using an unsigned short int (16bit)
		using ID = std::uint32_t;

		// Constructors
		Mark() :
			mAtomPos(-1),
			mIteratorID(-1)
		{
		}
		Mark(const Mark& Copy) :
			mAtomPos(Copy.mAtomPos),
			mIteratorID(Copy.mIteratorID)
		{
		}
		Mark(Mark&& Move) :
			mAtomPos(Move.mAtomPos),
			mIteratorID(Move.mIteratorID)
		{
			Move.mAtomPos = -1;
			Move.mIteratorID = -1;
		}

		// Construct a mark with a specified Atom Position and Iter ID
		Mark(const Position& AtomPos, const ID& IterID) :
			mAtomPos(AtomPos),
			mIteratorID(IterID)
		{
		}

		// Assignment operator
		Mark& operator=(const Mark& Copy)
		{
			mIteratorID = Copy.mIteratorID;
			mAtomPos = Copy.mAtomPos;

			return *this;
		}
		Mark& operator=(Mark&& Move)
		{
			mIteratorID = Move.mIteratorID;
			mAtomPos = Move.mAtomPos;

			Move.mIteratorID = -1;
			Move.mAtomPos = -1;

			return *this;
		}

		ID			mIteratorID;
		Position	mAtomPos;
	};

	// The "Iterator" keep track of data in the array
	struct Iterator
	{
		// Alias for an ID
		using ID = std::uint32_t;

		Iterator() :
			mParentVector(nullptr),
			mMarkPos(-1),
			mIteratorID(-1)
		{
		}

		// Copy constructor
		Iterator(const Iterator& Copy) :
			mParentVector(Copy.mParentVector),
			mMarkPos(Copy.mMarkPos),
			mIteratorID(Copy.mIteratorID)
		{
		}

		// Move constructor
		Iterator(Iterator&& Move) :
			mParentVector(Move.mParentVector),
			mMarkPos(Move.mMarkPos),
			mIteratorID(Move.mIteratorID)
		{
			Move.mParentVector = nullptr;
			Move.mMarkPos = -1;
			Move.mIteratorID = -1;
		}

		// Create an iterator starting from the data position
		Iterator(const Position& DataPosition, const TVector* ParentVector)
		{
			// If we are creating an end() Iterator
			mMarkPos = ParentVector->mAtomsVector[DataPosition].mMarkPos;
			mIteratorID = ParentVector->mMarksVector[mMarkPos].mIteratorID;
			mParentVector = ParentVector;
		}

		Iterator(const Position& MarkPos, const ID& IteratorID, const TVector* ParentVector) :
			mParentVector(ParentVector),
			mMarkPos(MarkPos),
			mIteratorID(IteratorID)
		{
		}

#pragma region Assignemt operators
		// Copy assignment
		Iterator& operator =(const Iterator& Copy)
		{
			mParentVector = Copy.mParentVector;
			mMarkPos = Copy.mMarkPos;
			mIteratorID = Copy.mIteratorID;

			return *this;
		}

		// Move assignment
		Iterator& operator =(Iterator&& Move)
		{
			mParentVector = Move.mParentVector;
			mMarkPos = Move.mMarkPos;
			mIteratorID = Move.mIteratorID;

			Move.mParentVector = nullptr;
			Move.mMarkPos = -1;
			Move.mIteratorID = -1;

			return *this;
		}

		// Addition assignment
		Iterator& operator +=(const Position& Offset)
		{
			*this = *this + Offset;

			return *this;
		}

		// Subtraction assignment
		Iterator& operator -=(const Position& Offset)
		{
			*this = *this - Offset;

			return *this;
		}
#pragma endregion

#pragma region Member Access
		// Indirection 
		Reference operator*()
		{
			// Check if this iterator points to valid Data
			assert(isValid());

			return mParentVector->referenceCast(mParentVector->mVectorData[mParentVector->getDataIndexFromIterator(*this)]);
		}

		// Address-of 
		Pointer operator&()
		{
			return &(operator*());
		}

		// Member of pointer 
		Pointer operator->()
		{
			return operator&();
		}
#pragma endregion

#pragma region Arithmetic operators
		// Pre-increment/Pre-decrement
		Iterator& operator++()
		{
			*this = *this + 1;
			return *this;
		}
		Iterator& operator--()
		{
			*this = *this - 1;
			return *this;
		}

		// Post-increment/Post-decrement
		Iterator operator++(int)
		{
			auto Temp = *this;
			*this = *this + 1;
			return Temp;
		}
		Iterator operator--(int)
		{
			auto Temp = *this;
			*this = *this - 1;
			return Temp;
		}

		// Addition	operator
		Iterator operator+(const Position& Offset)
		{
			return Iterator(mParentVector->getDataIndexFromIterator(*this) + Offset, mParentVector);
		}

		// Subtraction	operator
		Iterator operator-(const Position& Offset)
		{
			return Iterator(mParentVector->getDataIndexFromIterator(*this) - Offset, mParentVector);
		}
#pragma endregion

#pragma region Comparison operators
		bool operator ==(const Iterator& Right) const
		{
			return mParentVector->getDataIndexFromIterator(*this) == mParentVector->getDataIndexFromIterator(Right);
		}
		bool operator !=(const Iterator& Right) const
		{
			return !(*this == Right);
		}
		bool operator <(const Iterator& Right) const
		{
			return mParentVector->getDataIndexFromIterator(*this) < mParentVector->getDataIndexFromIterator(Right);
		}
		bool operator >(const Iterator& Right) const
		{
			return !(*this < Right);
		}
		bool operator <=(const Iterator& Right) const
		{
			return mParentVector->getDataIndexFromIterator(*this) <= mParentVector->getDataIndexFromIterator(Right);
		}
		bool operator >=(const Iterator& Right) const
		{
			return mParentVector->getDataIndexFromIterator(*this) >= mParentVector->getDataIndexFromIterator(Right);
		}
#pragma endregion

		// Check if the iterator is valid
		inline bool isValid()
		{
			// Check if this iterator ID is the same of the connectred MARK 
			return mParentVector->mMarksVector[mMarkPos].mIteratorID == mIteratorID;
		}

	private:

		// Get the connected data mark
		const Mark& getMark()
		{
			return mParentVector->mMarksVector[mMarkPos];
		}

	private:
		ID			mIteratorID;
		Position	mMarkPos;

		friend typename TVector;
		const TVector*	mParentVector;
	};

public:

	// Default constructor
	TVector() :
		mVectorSize(0u),
		mVectorCapacity(1u),
		mVectorData(nullptr),
		mIteratorDefaultID(0)
	{
		init();
	}

	// Default destructor
	~TVector()
	{
		if (mVectorData)
		{
			// Call the destructor for all the allocated element
			for (auto Index = 0u; Index < mVectorSize; ++Index)
				referenceCast(mVectorData[Index]).~Type();

			// Deallocate all the vector memory
			delete[] mVectorData;
			mVectorData = nullptr;
		}
	}

#pragma region Element access 

	// Access the first element
	Reference  front()
	{
		return referenceCast(mVectorData[0]);
	}
	CReference front() const
	{
		return referenceCast(mVectorData[0]);
	}

	// Access the last element
	Reference  back()
	{
		return referenceCast(mVectorData[mVectorSize - 1]);
	}
	CReference back() const
	{
		return referenceCast(mVectorData[mVectorSize - 1]);
	}

	// Direct access to the underlying array 
	Pointer data()
	{
		return pointerCast(mVectorData);
	}

	// Access specified element with bounds checking
	Reference  at(const Size& Index)
	{
		// Bound checking
		assert(Index < mVectorSize);

		return referenceCast(mVectorData[Index]);
	}
	CReference at(const Size& Index) const
	{
		// Bound checking
		assert(Index < mVectorSize);

		return referenceCast(mVectorData[Index]);
	}

	// Access specified element
	Reference  operator[](const Size& Index)
	{
		return referenceCast(mVectorData[Index]);
	}
	CReference operator[](const Size& Index) const
	{
		return referenceCast(mVectorData[Index]);
	}

#pragma endregion

#pragma region Iterators

	// Return the iterator to the first element in the vector
	Iterator begin()
	{
		return CIterator(0, this);
	}
	CIterator begin() const
	{
		return CIterator(0, this);
	}
	CIterator cbegin() const
	{
		return begin();
	}

	// Return the iterator to the past end element in the vector
	Iterator end()
	{
		return CIterator(mVectorSize, this);
	}
	CIterator end() const
	{
		return CIterator(mVectorSize, this);
	}
	CIterator cend() const
	{
		return end();
	}
#pragma endregion

#pragma region Capacity

	// Checks whether the container is empty 
	bool empty()
	{
		return !static_cast<bool>(mVectorSize);
	}

	// Returns the number of elements 
	const Size& size() const noexcept
	{
		return mVectorSize;
	}

	// Returns the maximum possible number of elements
	Size max_size() const noexcept
	{
		return ((size_t)(-1) / sizeof(Type));
	}

	// Reserves storage 
	void reserve(const Size& NewCapacity)
	{
		// Check if we are allocating more storage then we already have
		if (NewCapacity <= mVectorCapacity)
			return;

		// Reallocate the entire array
		growVector(NewCapacity);
	}

	// Returns the number of elements that can be held in currently allocated storage
	const Size& capacity() const noexcept
	{
		return mVectorCapacity;
	}

	// Reduces memory usage by freeing unused memory
	void shrink_to_fit()
	{
		// Exit early if the vector has the same capacity or less that the vector size
		if (mVectorCapacity <= mVectorSize)
			return;

		// Reallocate the entire array
		growVector(mVectorSize);
	}

#pragma endregion

#pragma region Modifiers

	// Clears the contents
	void clear() noexcept
	{
		// Destroy all the elements
		for (auto Index = 0u; Index < mVectorSize; ++Index)
			reinterpret_cast<Type*>(mVectorData + Index)->~Type();

		// Clean the atom vector
		mAtomsVector.clear();

		// Clean the marks vector (increase by one all the marks ID)
		mMarksVector.clear();
		++mIteratorDefaultID;

		// Reset the vector size
		mVectorSize = 0u;
		mVectorCapacity = 1u;

		init();
	}

	// inserts value before pos
	template<class... TArgs>
	Iterator emplace(CIterator& InsertPosition, TArgs&&... Args)
	{
		/*
		To insert an element at an arbitrary position:
		1) If we are inserting an element at the end use the emplaceBack function and exit the emplace function
		2) Check where in the data array we are inserting the value
		3) Check if we have enough space in the data array

		4) Shift all the data array past the insertion position to the right
		5) Create the new element in place at position pointed by the "InsertPosition" iterator

		6) Shift to the right all the iterator structure (atom, mark) used by the value past the insertion point, except for the last (that one is the end() iterator structure)
		7) Create the right iterator structure (atom, mark) in place of the end() iterator
		8) Create a new end() iterator
		9) Increase the vector size
		*/


		// If we are inserting an element at the end use the emplaceBack function and exit the emplace function
		if (InsertPosition == end())
			return emplaceBack(std::forward<TArgs>(Args)...);

		// Check where in the data array we are inserting the value
		auto Index = getDataIndexFromIterator(InsertPosition);

		// Check if we have enough space in the data array 
		if (mVectorSize + 1 > mVectorCapacity)
			growVector(mVectorCapacity * 2);

		// Shift all the data array past the insertion position to the right
		shiftArrayRight(Index, 1);

		// Create the new element in place at position pointed by the "InsertPosition" iterator
		new(mVectorData + Index) Type(std::forward<TArgs>(Args)...);

		// Shift to the right all the iterator structure (atom, mark) used by the value past the insertion point, except for the last (that one is the end() iterator structure)
		shiftAtomVectorRight(Index);
		shiftMarkVectorRight(Index);

		// Point the atom in the insert position to the pointed mark pos
		auto NewMarkPos = static_cast<Position>(mMarksVector.size()) - 1;
		mAtomsVector[Index].mMarkPos = NewMarkPos;
		mMarksVector[NewMarkPos] = { Index, mIteratorDefaultID };

		// Increase the vector size
		++mVectorSize;

		// Create the right iterator structure (atom, mark) in place of the end() iterator
		mAtomsVector.emplace_back(mVectorSize, NewMarkPos + 1);
		mMarksVector.emplace_back(mVectorSize, mIteratorDefaultID);

		// Return an iterator to the newly added iterator
		return Iterator(Index, this);
	}
	Iterator insert(CIterator& InsertPosition, const Type& Value)
	{
		// use the emplace function
		return emplace(InsertPosition, Value);
	}
	Iterator insert(CIterator& InsertPosition, Type&& Value)
	{
		// use the emplace function
		return emplace(InsertPosition, std::move(Value));
	}

	// Inserts count copies of the value before pos
	Iterator insert(CIterator& InsertPosition, Size Count, const Type& Value)
	{
		// Copy the passed iterator
		Iterator Temp = InsertPosition;
		auto InsertPosIndex = getDataIndexFromIterator(InsertPosition);

		// Create a "Count" number of "Value"
		for (auto Index = 0u; Index < Count; ++Index)
		{
			Temp = emplace(Temp, Value);
			++Temp;
		}

		// Return an iterator to the first added element
		return Iterator(InsertPosIndex, this);
	}

	// Inserts elements from range [first, last) before pos
	template<class InputIt>
	Iterator insert(CIterator& InsertPosition, InputIt First, InputIt Last)
	{
		// Copy the passed iterator
		Iterator Temp = InsertPosition;
		auto InsertPosIndex = getDataIndexFromIterator(InsertPosition);

		// Copy all the value in the range (First, Last)
		while (First < Last)
		{
			Temp = emplace(Temp, *(First++));
			++Temp;
		}

		// Return an iterator to the first added element
		return Iterator(InsertPosIndex, this);
	}

	// Inserts elements from initializer list ilist before pos
	Iterator insert(CIterator& InsertPosition, std::initializer_list<Type> IList)
	{
		// Copy the passed iterator
		Iterator Temp = InsertPosition;
		auto InsertPosIndex = getDataIndexFromIterator(InsertPosition);

		// Copy all the value in the range (First, Last)
		for (auto& Element : IList)
		{
			Temp = emplace(Temp, Element);

			++Temp;
		}

		// Return an iterator to the first added element
		return Iterator(InsertPosIndex, this);
	}

	// Removes specified elements from the container.
	Iterator erase(CIterator& DeletePosition)
	{
		/*
		To delete an element:
		1) Remove the element from the data array
		2) Increment the mIteratorID of the MARK to delete
		3) Shift to the left all the ATOMS after the atom to delete, and decrement the Position of connected Data in the shifted ATOMS
		4) Decrement the mAtomPos of the Mark pointed by the shifted ATOMS
		*/

		// If the delete position is the end() iterator skip this function now
		if (DeletePosition == end())
			return DeletePosition;

		// Get the index of the value to remove
		auto Index = getDataIndexFromIterator(DeletePosition);

		// Remove data from the data array by ...
		// Calling the destructor on the data to remove ...
		referenceCast(mVectorData[Index]).~Type();

		// Shifting the data array to the left ...
		shiftArrayLeft(Index, 1);

		// And erasing the last element (calling it's destructor)
		referenceCast(mVectorData[mVectorSize - 1]).~Type();

		// Invalidates the connected mark
		++getMarkFromIterator(DeletePosition).mIteratorID;

		// Delete the connected atom
		shiftAtomVectorLeft(Index, 1);
		mAtomsVector.pop_back();

		// Decrement the size of the vector
		--mVectorSize;

		// Set the last atom as the end()
		auto NewMarkPos = static_cast<Position>(mMarksVector.size()) - 1;
		mAtomsVector[mVectorSize] = { mVectorSize, NewMarkPos };
		mMarksVector[NewMarkPos] = { mVectorSize, mIteratorDefaultID };

		return Iterator(Index, this);
	}
	Iterator erase(CIterator& First, CIterator& Last)
	{
		// Use the erase function with one parameter
		Iterator Temp = Last;
		auto Index = getDataIndexFromIterator(First);

		while (--Temp > First)
			erase(Temp);

		// Return an iterator to the first added element
		return Iterator(Index, this);
	}

	// Create an element using the passed arguments at the end of the vector
	template<class... TArgs>
	Iterator emplaceBack(TArgs&&... Args)
	{
		/*
		To insert an element at the end:
		1) If we won't have enough space for a new element grows the vector
		2) Create the new element in place at the end of the data vector
		3) Create the right iterator structure (atom, mark) in place of the end() iterator
		4) Create a new end() iterator
		5) Increase the vector size
		*/

		// If we won't have enough space for a new element grows the vector 
		if (mVectorSize + 1 > mVectorCapacity)
			growVector(mVectorCapacity * 2);

		// Create the new element in place at the end of the data vector
		new(mVectorData + mVectorSize) Type(std::forward<TArgs>(Args)...);

		// Create the right iterator structure (atom, mark) in place of the end() iterator
		auto NewMarkPos = static_cast<Position>(mMarksVector.size()) - 1;
		mAtomsVector[mVectorSize] = { mVectorSize, NewMarkPos };
		mMarksVector[NewMarkPos] = { mVectorSize, mIteratorDefaultID };

		// Increase the vector size
		++mVectorSize;

		// Create a new end() iterator
		mAtomsVector.emplace_back(mVectorSize, NewMarkPos + 1);
		mMarksVector.emplace_back(mVectorSize, mIteratorDefaultID);

		// Return an iterator to the newly added iterator
		return Iterator(mVectorSize - 1, this);
	}

	// Copy the passed element in at the end of the vector
	Iterator pushBack(const Type& Element)
	{
		// Use the emplaceBack function to emplace an element by passing an Element in the function call as a copy
		return emplaceBack(Element);
	}

	// Move the passed element in at the end of the vector
	Iterator pushBack(Type&& Element)
	{
		// Use the emplaceBack function to emplace an element by passing an Element in the function call as a move copy
		return emplaceBack(std::move(Element));
	}

	// Remove the last element in the vector
	void popBack()
	{	
		// Index of the data to remove
		auto Index = mVectorSize - 1;

		// Remove data from the data array by ...
		// Calling the destructor on the data to remove ...
		referenceCast(mVectorData[Index]).~Type();

		// Shifting the data array to the left ...
		shiftArrayLeft(Index, 1);

		// And erasing the last element (calling it's destructor)
		referenceCast(mVectorData[Index - 1]).~Type();

		// Invalidates the connected mark
		++getMarkFromAtom(mAtomsVector.back()).mIteratorID;

		// Delete the connected atom
		shiftAtomVectorLeft(Index, 1);
		mAtomsVector.pop_back();

		// Decrement the size of the vector
		--mVectorSize;

		// Set the last atom as the end()
		auto NewMarkPos = static_cast<Position>(mMarksVector.size()) - 1;
		mAtomsVector[mVectorSize] = { mVectorSize, NewMarkPos };
		mMarksVector[NewMarkPos] = { mVectorSize, mIteratorDefaultID };
	}

#pragma endregion

private:

	//  Cast function utility
	inline Reference referenceCast(Data& DataToCast)
	{
		return reinterpret_cast<Reference>(DataToCast);
	}
	inline const Reference referenceCast(Data& DataToCast) const
	{
		return reinterpret_cast<Reference>(DataToCast);
	}
	inline Pointer pointerCast(Data* DataToCast)
	{
		return reinterpret_cast<Pointer>(DataToCast);
	}
	inline const Pointer pointerCast(Data* DataToCast) const
	{
		return reinterpret_cast<Pointer>(DataToCast);
	}

	void init()
	{
		// Create a basic pointer for the data
		mVectorData = new Data[mVectorCapacity];

		// Create one iterator structure (atom, mark), this is our first iterator, and for now also the end() iterator
		mAtomsVector.emplace_back(0, 0);
		mMarksVector.emplace_back(0, 0);
	}

	// Grow the vector by a specific amount
	void growVector(const Size& NewCapacity)
	{
		// Create a new temp array
		Data* TempArray = new Data[NewCapacity];

		// Move the old array in the new array
		for (auto Index = 0u; Index < mVectorSize; ++Index)
			new(TempArray + Index) Type(std::move(referenceCast(mVectorData[Index])));

		// Delete the old data
		for (auto Index = 0u; Index < mVectorSize; ++Index)
			referenceCast(mVectorData[Index]).~Type();

		delete[] mVectorData;

		// Assign the temp array to the vector data
		mVectorData = TempArray;

		// Update the vector capacity
		mVectorCapacity = NewCapacity;
	}

	// Shift atoms to the right
	void shiftAtomVectorRight(const Position& StartPosition)
	{
		// Right shift the atoms
		auto& EndOfShift = mAtomsVector.rbegin();
		auto& BeginOfShift = EndOfShift + (mVectorSize - StartPosition);
		for (auto& RightAtom = EndOfShift, LeftAtom = EndOfShift + 1; RightAtom != BeginOfShift; ++RightAtom, ++LeftAtom)
		{
			RightAtom->mMarkPos = LeftAtom->mMarkPos;
			RightAtom->mDataPos = LeftAtom->mDataPos + 1;
		}
	}

	// Shift marks to the right
	void shiftMarkVectorRight(const Position& StartPosition)
	{
		auto NumberOfMarks = mMarksVector.size();
		for (auto Index = StartPosition + 1; Index < NumberOfMarks; ++Index)
			++getMarkFromAtom(mAtomsVector[Index]).mAtomPos;
	}

	// Shift array to the right from a starting position for a specified number of elements
	void shiftArrayRight(const Position& StartPosition, const Size& NoOfElement)
	{
		// Right shift start
		auto EndOfShift = (mVectorSize + NoOfElement) - 1;

		// Left shift start
		auto BeginOfShift = mVectorSize - 1;

		// Right shift the data to the right
		for (auto RightIndex = EndOfShift, LeftIndex = BeginOfShift, Counter = mVectorSize; Counter > StartPosition; --RightIndex, --LeftIndex, --Counter)
			std::memcpy(mVectorData + RightIndex, mVectorData + LeftIndex, sizeof(Data));
		//mVectorData[RightIndex] = std::move(mVectorData[LeftIndex]);
	}

	// Shift array to the right from a starting position for a specified number of elements
	void shiftArrayLeft(const Position& StartPosition, const Size& NoOfElement)
	{
		for (auto LeftIndex = StartPosition, RightIndex = StartPosition + NoOfElement; RightIndex < mVectorSize; ++LeftIndex, ++RightIndex)
			std::memcpy(mVectorData + LeftIndex, mVectorData + RightIndex, sizeof(Data));
	}

	// Shift the atom array to the left
	void shiftAtomVectorLeft(const Position& StartPosition, const Size& NoOfElement)
	{
		auto EndOfShift = mAtomsVector.size();

		// Shift the atoms vector to the left by a specified amount
		for (auto LeftIndex = StartPosition, RightIndex = StartPosition + NoOfElement; RightIndex < EndOfShift; ++LeftIndex, ++RightIndex)
		{
			// Copy the content of X + 1 -> X
			mAtomsVector[LeftIndex] = mAtomsVector[RightIndex];

			// Decrement the value of data pos to match the position of the Data we are pointed in the array
			mAtomsVector[LeftIndex].mDataPos -= NoOfElement;

			// Decrement the mAtomPos of the connected Mark to match the new position of the shifted Atom
			mMarksVector[mAtomsVector[LeftIndex].mMarkPos].mAtomPos -= NoOfElement;
		}
	}

	Mark& getMarkFromIterator(const Iterator& SourceIterator)
	{
		return mMarksVector[SourceIterator.mMarkPos];
	}
	const Mark& getMarkFromIterator(const Iterator& SourceIterator) const
	{
		return mMarksVector[SourceIterator.mMarkPos];
	}

	// Atom/Mark exchange
	Mark& getMarkFromAtom(const Atom& SourceAtom)
	{
		return mMarksVector[SourceAtom.mMarkPos];
	}
	const Mark& getMarkFromAtom(const Atom& SourceAtom) const
	{
		return mMarksVector[SourceAtom.mMarkPos];
	}

	// Get the actual position in the data array from an iterator
	const Position& getDataIndexFromIterator(const Iterator& SourceIterator) const
	{
		return mAtomsVector[getMarkFromIterator(SourceIterator).mAtomPos].mDataPos;
	}

private:
	Data*	mVectorData;
	Size	mVectorSize;
	Size	mVectorCapacity;
	Position	mIteratorDefaultID;

	// 
	std::vector<Atom>	mAtomsVector;
	std::vector<Mark>	mMarksVector;
};
