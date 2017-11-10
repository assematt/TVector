## TVector
### A simple vector class with always valid iterator.

This simple class aims to solve the std::vector iterator invalidation problem when the vector internally reallocates memory, or when an element is inserted or deleted.

**TVector** iterators will still be valid even if reallocation occurs or we insert an element in the middle. In case of deletion, only the iterator to the removed element in the vector will be invalidated. 

The class introduces two new structures to archive that, **Atom** and **Mark**. These two classes provide the necessary indirection between the iterators the internal array data. 

#### Atom
The **Atom** struct it's a 64 bit structure containing two 32 bit unsigned integer variables. The first variable called "*mDataPos*" store the index of the data in the **TVector** internal array we are keeping track of. The second variable called "*mMarkPos*" store the index of the connected Mark structure in the Marks vector.  This structure provides the first level of indirection.  

#### Mark
The **Mark** struct it's a 64 bit structure containing two 32 bit unsigned integer variables. The first variable called "*mIteratorID*" store the iterator ID, this is used for validation. The second variable called "*mAtomPos*" store the index of the connected **Atom** structure in the Atoms vector.  This structure provides the second level of indirection. 

#### Iterator
The **Iterator** struct is a 128bit (x64 architecture) or 96bit (x86 architecture) structure containing two 32 bit unsigned integer variables and a pointer to a parent TVector class. The first variable called "*mIteratorID*" store the iterator ID, this is used for validation, if it's equal to the iterator ID value of the connected **Mark** the iterator is valid. The second variable called "*mMarkPos*" store the index of the connected **Mark** structure in the Marks vector. The third variable called "*mParentVector*" it's a pointer to the parent **TVector** class.

This project was inspired by [Vittorio Romeo](https://github.com/SuperV1234) [handle management system](https://www.youtube.com/watch?v=_-KSlhppzNE "handle management system").