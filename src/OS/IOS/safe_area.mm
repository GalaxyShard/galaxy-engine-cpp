#import <UIKit/UIKit.h>

extern "C" void get_safe_area_platform(float *top, float *bottom, float *left, float *right)
{
    UIWindow *window = UIApplication.sharedApplication.windows.firstObject;
    *top = window.safeAreaInsets.top;
    *bottom = window.safeAreaInsets.bottom;
    *left = window.safeAreaInsets.left;
    *right = window.safeAreaInsets.right;
}