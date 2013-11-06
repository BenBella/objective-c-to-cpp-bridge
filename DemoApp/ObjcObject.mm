//
//  ObjcObject.m
//  DemoApp
//
//  Created by Lukáš Andrlík on 11/6/13.
//  Copyright (c) 2013 Lukáš Andrlík. All rights reserved.
//

#import "ObjcObject.h"
#include "CppObject.h"

@interface ObjcObject () {

    CppObject* _wrapped; // Will be initialized to NULL by alloc.
}

@end

@implementation ObjcObject {
 
    //CppObject wrapped;
}

- (id)init
{
    self = [super init];
    if (self)
    {
        
        _wrapped = new CppObject(); // Call to CppObject constructor;
        if (!_wrapped) self = nil;
    }
    return self;
}

- (void)dealloc
{
    if (_wrapped) _wrapped->~CppObject(); // Call to CppObject destructor;
    // or delete _wrapped
    // omit if using ARC [super dealloc];
}

- (void)exampleMethodWithString:(NSString*)str
{
    // NOTE: if str is nil this will produce an empty C++ string
    // instead of dereferencing the NULL pointer from UTF8String.
    
    std::string cpp_str([str UTF8String], [str lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
    string result = _wrapped->exampleMethod(cpp_str);
    NSString* resultStringObject = [NSString stringWithUTF8String:result.c_str()];
    NSLog(@"NSString object: %@",resultStringObject);
}

@end
