#include "FilesystemResourceLocator.hpp"
#include <fstream>

#if __APPLE__
#ifndef __OBJC__
#error This file must be compiled as an Objective-C++ source file!
#endif
#import <Foundation/Foundation.h>

// Taken directly from the app template on Mac
inline static std::string getResourceDirectory()
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    
    std::string rpath;
    NSBundle* bundle = [NSBundle mainBundle];
    
    if (bundle == nil) {
#ifdef DEBUG
        NSLog(@"bundle is nil... thus no resources path can be found.");
#endif
    } else {
        NSString* path = [bundle resourcePath];
        rpath = [path UTF8String];
    }
    
    [pool drain];
    
    return rpath;
}
#else
inline static std::string getResourceDirectory()
{
    return getExecutableDirectory() + "/Resources";
}
#endif

FilesystemResourceLocator::FilesystemResourceLocator() : basename(getResourceDirectory())
{
}

std::unique_ptr<sf::InputStream> FilesystemResourceLocator::getResource(std::string name)
{
    std::string fullname = basename + '/' + name;

    std::unique_ptr<sf::FileInputStream> ptr{new sf::FileInputStream()};
    if (!ptr->open(fullname))
        throw FileNotFound(name);

    return std::unique_ptr<sf::InputStream>{ptr.release()};
}
