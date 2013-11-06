//
//  ObjcObject.h
//  DemoApp
//
//  Created by Lukáš Andrlík on 11/6/13.
//  Copyright (c) 2013 Lukáš Andrlík. All rights reserved.
//

#import <Foundation/Foundation.h>
//#import "CppObject.h"

@interface ObjcObject : NSObject
//{
//    CppObject _wrapped;
//}

/*
 RECAP OF THE PROBLEM (C++ to Objective-C bridge)
 ------------------------------------------------
 
 Let's say you have some existing C++ code, a library perhaps, and you want to use it 
 in an Objective-C application. Typically, your C++ code will define some class you'd like to use. 
 You could switch your whole project to Objective-C++ by renaming all the .m files to .mm, and 
 freely mix C++ and Objective-C. That's certainly an option, but the two worlds are quite different,
 so such "deep" mixing can become awkward.
 
 So usually you'll want to wrap the C++ types and functions with Objective-C equivalents 
 that you can use in the rest of your project. Let's say you have a C++ class called CppObject, 
 defined in CppObject.h:
 
 #include <string>
 
 class CppObject
 {
 public:
    void ExampleMethod(const std::string& str);
    // constructor, destructor, other members, etc.
 };
 
 You can have C++-typed members in an Objective-C class, so the typical first attempt 
 is to do this with your wrapper class, ObjcObject - in ObjcObject.h:
 
 #import <Foundation/Foundation.h>
 #import "CppObject.h"
 
 @interface ObjcObject : NSObject {
    CppObject wrapped;
 }
 - (void)exampleMethodWithString:(NSString*)str;
 // other wrapped methods and properties
 @end
 
 And then implementing the methods in ObjcObject.mm. Many are then surprised to get preprocessor 
 and compile errors in ObjcObject.h and CppObject.h when they #import "ObjcObject.h" from a pure 
 Objective-C (.m) file directly or indirectly via another header (.h) file. The thing to bear in mind is that the 
 preprocessor basically just does text substitution, so #include and #import directives are essentially 
 equivalent to recursively copy-and-pasting the contents of the file in question into the location 
 of the directive. So in this example, if you #import "ObjcObject.h" you're essentially 
 inserting the following code:
 
 // [lots and lots of Objective-C code from Foundation/Foundation.h]
 // [fail to include <string>] as that header is not in the include path outside of C++ mode
 class CppObject
 {
 public:
    void ExampleMethod(const std::string& str);
    // constructor, destructor, other members, etc.
 };
 
 @interface ObjcObject : NSObject {
    CppObject wrapped;
 }
 - (void)exampleMethodWithString:(NSString*)str;
 // other wrapped methods and properties
 @end
 
 The compiler will get enormously confused by class CppObject and the block following it, 
 as that's simply not valid Objective-C syntax. The error will typically be something like
 
 Unknown type name 'class'; did you mean 'Class'?
 as there is no class keyword in Objective-C. So to be compatible with Objective-C, 
 our Objective-C++ class's header file must contain only Objective-C code, 
 absolutely no C++ - this mainly affects types in particular (like the CppObject class type here).
 
 
 KEEPING YOUR HEADERS CLEAN
 --------------------------
 
 With Clang, there is new way to keep C++ out of your Objective-C headers: ivars in class extensions.
 
 Class extensions (not to be confused with categories) have existed in Objective-C for a while: they 
 let you declare additional parts of the class's interface outside the public header before the 
 @implementation block. As such, the only sensible place to put them is just above said block, e.g. ObjcObject.mm:
 
 #import "ObjcObject.h"
 
 @interface ObjcObject () // note the empty parentheses
 - (void)methodWeDontWantInTheHeaderFile;
 @end
 
 @implementation ObjcObject
 // etc.
 
 This much already worked with GCC, but with clang, you can also add an ivar block to it. 
 This means we can declare any instance variables with C++ types in the extension, or at 
 the start of the @implementation block. In our case, we can reduce the ObjcObject.h file to this:
 
 #import <Foundation/Foundation.h>
 
 @interface ObjcObject : NSObject
 - (void)exampleMethodWithString:(NSString*)str;
 // other wrapped methods and properties
 @end
 
 The missing parts all move to the class extension in the implementation file (ObjcObject.mm):
 
 #import "ObjcObject.h"
 #import "CppObject.h"
 
 @interface ObjcObject () {
    CppObject wrapped;
 }
 @end
 
 @implementation ObjcObject
 
 - (void)exampleMethodWithString:(NSString*)str{
    // NOTE: if str is nil this will produce an empty C++ string
    // instead of dereferencing the NULL pointer from UTF8String.
    std::string cpp_str([str UTF8String], [str lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
    wrapped.ExampleMethod(cpp_str);
 }
 
 Alternatively, if we don't need the interface extension to declare any extra properties and methods, 
 the ivar block can also live at the start of the @implementation:
 
 #import "ObjcObject.h"
 #import "CppObject.h"
 
 @implementation ObjcObject {
    CppObject wrapped;
 }
 
 - (void)exampleMethodWithString:(NSString*)str
 {
    // NOTE: if str is nil this will produce an empty C++ string
    // instead of dereferencing the NULL pointer from UTF8String.
    std::string cpp_str([str UTF8String], [str lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
    wrapped.ExampleMethod(cpp_str);
 }
 
 Either way, we now #import "ObjcObject.h" to our heart's content and use ObjcObject like 
 any other Objective-C class. The CppObject instance for the wrapped ivar will be constructed 
 using the default constructor when you alloc (not init) an ObjcObject, the destructor will 
 be called on dealloc. This often isn't what you want, particularly if there isn't a (public) default 
 constructor at all, in which case the code will fail to compile.
 
 
 MENAGING THE WRAPPED C++ OBJECT'S LIFECYCLE
 -------------------------------------------
 
 The solution is to manually trigger construction via the new keyword, e.g.
 
 @interface ObjcObject () {
    CppObject* wrapped; // Pointer! Will be initialised to NULL by alloc.
 }
 @end
 
 @implementation ObjcObject
 
 - (id)initWithSize:(int)size {
 
    self = [super init];
    if (self)
    {
        wrapped = new CppObject(size);
        if (!wrapped) self = nil;
    }
    return self;
 }
 //...
 
 If using C++ exceptions, you may want to wrap the construction in a try {...} catch {...} block 
 and handle any construction errors. With explicit construction, 
 we also need to explicitly destroy the wrapped object:
 
 - (void)dealloc
 {
    delete wrapped;
    [super dealloc]; // omit if using ARC
 }
 
 Note that the extra level of indirection involves an extra memory allocation. 
 Objective-C heavily allocates and frees memory all over the place, so this one extra 
 allocation shouldn't be a big deal. If it is, you can use placement new instead, and 
 reserve memory within the Objective-C object via an extra char wrapped_mem[sizeof(CppObject)]; ivar,
 creating the instance using wrapped = new(wrapped_mem) CppObject(); and destroying it via an explicit 
 destructor call: if (wrapped) wrapped->~CppObject();. As with any use of placement new, though,
 you'd better have a good reason for it. Placement new returns a pointer to the constructed object.
 I would personally keep that (typed) pointer in an ivar just as with regular new. 
 The address will normally coincide with the start of the char array,
 so you could get away with casting that instead.
 
 WRAPPING UP
 -----------
 
 Now you'll probably want to wrap a bunch of member functions with Objective-C methods, 
 and public fields with properties whose getters and setters forward to the C++ object. 
 Make sure that your wrapper methods only return and take parameters with C or Objective-C types. 
 You may need to do some conversions or wrap some more C++ types. Don't forget Objective-C's special 
 nil semantics don't exist in C++: NULL pointers must not be dereferenced.

*/
 
- (void)exampleMethodWithString:(NSString*)str;
// other wrapped methods and properties

@end
