#include <TVector.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

template <typename T, typename U>
bool testValue(T Expected, U Received)
{
	// Check whatever the value are equal
	bool Test = Expected == Received;

	// Debug message
	if (Test)
		cout << "passed." << endl;
	else
		cout << "failed. Expected '" << Expected << "', received '" << Received << "'." << endl;

	return Test;	
}

int main()
{
	// Create 2 vectors
	TVector<char> CustomVector;
	vector<char> StdVector;

	char TestLetter = 'D';
	vector<char> TestRangeInsert {'F', 'G', 'H'};
	
	
	// Check every single function
	vector<bool> TestResults;

	// Check push_back
	cout << "Testing pushBack function: ";
	CustomVector.pushBack('A');
	StdVector.push_back('A');
	TestResults.push_back(testValue(StdVector[0], CustomVector[0]));

	// Check emplace_back
	cout << "Testing emplaceBack function: ";
	CustomVector.emplaceBack('B');
	StdVector.emplace_back('B');
	TestResults.push_back(testValue(StdVector[1], CustomVector[1]));

	// Check insert functions
	cout << "Testing insert(Iterator, Value&&) function: ";
	auto ResultCustom = CustomVector.insert(CustomVector.begin(), 'C');
	auto ResultStd = StdVector.insert(StdVector.begin(), 'C');
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	cout << "Testing insert(Iterator, Value&) function: ";
	ResultCustom = CustomVector.insert(CustomVector.end(), TestLetter);
	ResultStd = StdVector.insert(StdVector.end(), TestLetter);
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	cout << "Testing insert(Iterator, Count, Value&) function: ";
	ResultCustom = CustomVector.insert(CustomVector.begin() + 2, 5, 'E');
	ResultStd = StdVector.insert(StdVector.begin() + 2, 5, 'E');
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	cout << "Testing insert(Iterator, InputIt, InputIt) function: ";
	ResultCustom = CustomVector.insert(CustomVector.end() - 1, TestRangeInsert.begin(), TestRangeInsert.end());
	ResultStd = StdVector.insert(StdVector.end() - 1, TestRangeInsert.begin(), TestRangeInsert.end());
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	cout << "Testing insert(Iterator, std::initializer_list) function: ";
	ResultCustom = CustomVector.insert(CustomVector.begin(), { 'I', 'L', 'M', 'N' });
	ResultStd = StdVector.insert(StdVector.begin(), { 'I', 'L', 'M', 'N' });
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	// Check emplace function
	cout << "Testing emplace function: ";
	ResultCustom = CustomVector.emplace(CustomVector.begin() + 5, 'O');
	ResultStd = StdVector.emplace(StdVector.begin() + 5, 'O');
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	// Check popBack
	cout << "Testing popBack function: ";
	CustomVector.popBack();
	StdVector.pop_back();
	TestResults.push_back(testValue(StdVector.back(), CustomVector.back()));

	// Check erase(iterator)
	cout << "Testing erase(Iterator) function: ";
	ResultCustom = CustomVector.erase(CustomVector.end() - 5);
	ResultStd = StdVector.erase(StdVector.end() - 5);
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	// Check erase(iterator, iterator)
	cout << "Testing erase(Iterator, Iterator) function: ";
	ResultCustom = CustomVector.erase(CustomVector.end() - 5, CustomVector.end() - 1);
	ResultStd = StdVector.erase(StdVector.end() - 5, StdVector.end() - 1);
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	// Check operator[]
	cout << "Testing operator[]:" << endl;
	cout << "Testing Vector:	";
	for (auto Index = 0u; Index < CustomVector.size(); ++Index)
		cout << CustomVector[Index] << " ";
	cout << endl;

	cout << "Std Vector:	";
	for (auto Index = 0u; Index < StdVector.size(); ++Index)
		cout << StdVector[Index] << " ";
	cout << endl;

	// Check at() function
	cout << "Testing at() function: ";
	auto CustomItem = CustomVector.at(6);
	auto StdItem = StdVector.at(6);
	TestResults.push_back(testValue(StdItem, CustomItem));

	// Check front() function
	cout << "Testing front() function: ";
	CustomItem = CustomVector.front();
	StdItem = StdVector.front();
	TestResults.push_back(testValue(StdItem, CustomItem));

	// Check back() function
	cout << "Testing back() function: ";
	CustomItem = CustomVector.back();
	StdItem = StdVector.back();
	TestResults.push_back(testValue(StdItem, CustomItem));

	// check begin/end functions
	cout << "Testing begin() function: ";
	ResultCustom = CustomVector.begin();
	ResultStd = StdVector.begin();
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	cout << "Testing end() function: ";
	ResultCustom = CustomVector.end();
	ResultStd = StdVector.end();
	TestResults.push_back(testValue(*(ResultStd - 1), *(ResultCustom - 1)));

	// Check empty() function
	cout << "Testing empty() function: ";
	bool CustomEmpty = CustomVector.empty();
	bool StdEmpty = StdVector.empty();
	TestResults.push_back(testValue(StdEmpty, CustomEmpty));

	// Check size() function
	cout << "Testing size() function: ";
	auto CustomSize = CustomVector.size();
	auto StdSize = StdVector.size();
	TestResults.push_back(testValue(StdSize, CustomSize));

	// Check max_size() function
	cout << "Testing max_size() function: ";
	CustomSize = CustomVector.max_size();
	StdSize = StdVector.max_size();
	TestResults.push_back(testValue(StdSize, CustomSize));

	// Check capacity() function
	cout << "Testing capacity() function: ";
	CustomSize = CustomVector.capacity();
	StdSize = StdVector.capacity();
	TestResults.push_back(testValue(StdSize, CustomSize));

	// Check shrink_to_fit() function
	cout << "Testing shrink_to_fit() function: ";
	CustomVector.shrink_to_fit();
	StdVector.shrink_to_fit();
	TestResults.push_back(testValue(StdVector.capacity(), CustomVector.capacity()));

	// Check reserve() function
	cout << "Testing reserve() function: ";
	CustomVector.reserve(CustomSize * 2);
	StdVector.reserve(StdSize * 2);
	TestResults.push_back(testValue(StdVector.capacity(), CustomVector.capacity()));

	// Check data
	cout << "Testing data" << endl;
	cout << "Testing Vector:	";
	for (auto Index = 0u; Index < CustomVector.size(); ++Index)
		cout << *(CustomVector.data() + Index) << " ";
	cout << endl;

	cout << "Std Vector:	";
	for (auto Index = 0u; Index < StdVector.size(); ++Index)
		cout << *(StdVector.data() + Index) << " ";
	cout << endl;

	// Test iterator functions
	ResultCustom = CustomVector.begin();
	ResultStd = StdVector.begin();

	// Post-increment
	cout << "Testing post-increment Iterator function: ";
	auto ResultCustom2 = ResultCustom++;
	auto ResultStd2 = ResultStd++;
	TestResults.push_back(testValue(*ResultStd2, *ResultCustom2));

	// Pre-increment
	cout << "Testing pre-increment Iterator function: ";
	++ResultCustom;
	++ResultStd;
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	// Addition assignment
	cout << "Testing addition assignment Iterator function: ";
	ResultCustom += 5;
	ResultStd += 5;
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	// Post-decrement
	cout << "Testing post-decrement Iterator function: ";
	ResultCustom2 = ResultCustom--;
	ResultStd2 = ResultStd--;
	TestResults.push_back(testValue(*ResultStd2, *ResultCustom2));

	// Pre-decrement
	cout << "Testing pre-decrement Iterator function: ";
	--ResultCustom;
	--ResultStd;
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	// Subtraction assignment
	cout << "Testing subtraction assignment Iterator function: ";
	ResultCustom -= 2;
	ResultStd -= 2;
	TestResults.push_back(testValue(*ResultStd, *ResultCustom));

	// Check clear() function
	cout << "Testing clear() function: ";
	CustomVector.clear();
	StdVector.clear();
	TestResults.push_back(testValue(StdVector.size(), CustomVector.size()));

	// Count the passed test
	auto PassedTests = count(TestResults.begin(), TestResults.end(), true);

	std::cout << "Passed " << PassedTests << " out of " << TestResults.size() << endl;
	std::cout << "---------------------------------------------------" << std::endl;
	
	// 
	system("PAUSE");

	// Everything went OK
	return 0;
}