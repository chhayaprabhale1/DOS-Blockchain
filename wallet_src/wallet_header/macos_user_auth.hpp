#pragma once

#include <CoreFoundation/CoreFoundation.h>

//ask for user authentication and call callback with true/false once compelte. 
extern "C" void macos_user_auth(void(*cb)(int, void*), void* cb_userdata, CFStringRef message);
