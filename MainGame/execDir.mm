#include "execDir.hpp"
#include <cstdio>
#include <vector>
#include <algorithm>

#import <Foundation/Foundation.h>

// Taken directly from the app template on Mac
std::string getExecutableDirectory()
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    
    std::string rpath;
    NSBundle* bundle = [NSBundle mainBundle];
    
    if (bundle == nil) {
#ifdef DEBUG
        NSLog(@"bundle is nil... thus no executable dir can be found.");
#endif
    } else {
        NSString* path = [bundle executablePath];
        rpath = [path UTF8String];
    }
    
    [pool drain];
    
    return rpath.substr(0, rpath.find_last_of('/'));
}

std::vector<std::string> getAllFilesInDir(std::string dir)
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    NSDirectoryEnumerator* enumerator = [[NSFileManager defaultManager] enumeratorAtPath:[NSString stringWithUTF8String:dir.c_str()]];
    
    std::vector<std::string> list;
    NSString* filename;
    while ((filename = [enumerator nextObject]))
        list.emplace_back(filename.UTF8String);
    
    [pool drain];
    
    return list;
}
