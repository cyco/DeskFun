//
//  DASpeechContoller.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface DASpeechContoller : NSWindowController
+ (instancetype)showText:(NSString*)text atPoint:(NSPoint)p ofView:(NSView*)view;

@property (assign) IBOutlet NSTextView *textView;
@property (assign) IBOutlet NSButton *closeButton;
@property (assign) IBOutlet NSButton *scrollUpButton;
@property (assign) IBOutlet NSButton *scrollDownButton;

- (IBAction)scrollUp:(id)sender;
- (IBAction)scrollDown:(id)sender;
- (IBAction)close:(id)sender;
@end
