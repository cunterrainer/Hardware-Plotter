#import <Foundation/NSObjCRuntime.h>
#import <Cocoa/Cocoa.h>

void MessageBoxErrorMacOS(const char* message)
{
    NSString* msg = [[NSString alloc] initWithUTF8String:message];
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"OK"];
    [alert setMessageText:@"Error"];
    [alert setInformativeText:msg];
    [alert setAlertStyle:NSAlertStyleCritical];

    [alert runModal];
    [alert release];
    [msg release];
    //if ([alert runModal] == NSAlertFirstButtonReturn) {
}
