//
//  CppObject.cpp
//  DemoApp
//
//  Created by Lukáš Andrlík on 11/6/13.
//  Copyright (c) 2013 Lukáš Andrlík. All rights reserved.
//

#include "CppObject.h"


CppObject::CppObject () {
    
    cout<<"CppObject construct\n";
};

CppObject::~CppObject () {
    
    cout<<"CppObject destruct\n";
};

string CppObject::exampleMethod(const std::string& str) {
    
    char buffer[20];
    std::size_t length = str.copy(buffer,6,5);
    buffer[length]='\0';
    std::cout << "CppObject method call with parameter:" + str + " & buffer after copy contains: " << buffer << "\n";
    return buffer;
};

