// ZTester.cpp : Defines the entry point for the console application.
//

#include <functional>
#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>

#include "stdafx.h"
#include "Delegate.hpp"
#include "MulticastDelegate.hpp"
#include "InterfaceWrapper.hpp"

#include "ServiceResolver.hpp"

#include "Arc_Assert.hpp"
#include "PropertyHelper.hpp"

struct MyString
{
	char* data;
	int length;
};

std::vector<MyString>* const MakeStrings()
{
	auto* strings = new std::vector<MyString>();
	
	strings->push_back(MyString());
	strings->push_back(MyString());
	strings->push_back(MyString());

	return strings;
}

int _tmain(int argc, _TCHAR* argv[])
{
	auto cStrings = MakeStrings();

	MyString test = (*cStrings)[0];

	test.data = "cont?";

	

	return 0;
}
