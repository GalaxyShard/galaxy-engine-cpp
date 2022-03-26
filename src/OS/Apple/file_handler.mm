#import <Foundation/Foundation.h>

// https://stackoverflow.com/questions/1061005/calling-objective-c-method-from-c-member-function
const char *get_resource_path_platform()
{
    return [[[NSBundle mainBundle]resourcePath]UTF8String];
}

// https://stackoverflow.com/questions/8883102/obtain-bundle-identifier-programmatically
const char *get_bundle_identifier()
{
    return [[[NSBundle mainBundle]bundleIdentifier]UTF8String];
}