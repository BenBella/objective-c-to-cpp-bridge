  C++ to Objective-C bridge (BRIDGE DESIGN PATTERN or PIMPL idiom)
 -----------------------------------------------------------------
 
 Let's say you have some existing C++ code, a library perhaps, and you want to use it 
 in an Objective-C application. Typically, your C++ code will define some class you'd like to use. 
 You could switch your whole project to Objective-C++ by renaming all the .m files to .mm, and 
 freely mix C++ and Objective-C. That's certainly an option, but the two worlds are quite different,
 so such "deep" mixing can become awkward.
 
 So usually you'll want to wrap the C++ types and functions with Objective-C equivalents 
 that you can use in the rest of your project. 
 
 See comments..