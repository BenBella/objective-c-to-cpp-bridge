//
//  CppObject.h
//  DemoApp
//
//  Created by Lukáš Andrlík on 11/6/13.
//  Copyright (c) 2013 Lukáš Andrlík. All rights reserved.
//

#ifndef __DemoApp__File__
#define __DemoApp__File__

#include <iostream>
#include <string>

using namespace std;

class CppObject
{
public:
    CppObject ();
    ~CppObject ();
    string exampleMethod(const std::string& str);
};

#endif /* defined(__DemoApp__File__) */
