#import <ChatRoomCallbackI.h>

@interface ChatRoomCallbackI()

@property (nonatomic, retain) id target;

@end

@implementation ChatRoomCallbackI

@synthesize target;

-(id)initWithTarget:(id)t
{
    if((self = [super init]))
    {
        self.target = t;
    }
    return self;
}
+(id)chatRoomCallbackWithTarget:(id)target
{
    return [[[ChatRoomCallbackI alloc] initWithTarget:target] autorelease];
}

-(void) init:(ICEMutableStringSeq *)users current:(ICECurrent *)current
{
    SEL selector = @selector(initUsers:);
    NSMethodSignature* sig = [[target class] instanceMethodSignatureForSelector:selector];
    NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:sig];
    [invocation setTarget:target];
    [invocation setSelector:selector];
    [invocation setArgument:&users atIndex:2];
    [invocation retainArguments];
    
    [invocation performSelectorOnMainThread:@selector(invokeWithTarget:) withObject:target waitUntilDone:NO];
}

-(void) send:(ICELong)timestamp name:(NSMutableString *)name message:(NSMutableString *)message current:(ICECurrent *)current
{
    SEL selector = @selector(send:name:message:);
    NSMethodSignature* sig = [[target class] instanceMethodSignatureForSelector:selector];
    NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:sig];
    [invocation setTarget:target];
    [invocation setSelector:selector];
    [invocation setArgument:&timestamp atIndex:2];
    [invocation setArgument:&name atIndex:3];
    [invocation setArgument:&message atIndex:4];
    [invocation retainArguments];

    [invocation performSelectorOnMainThread:@selector(invokeWithTarget:) withObject:target waitUntilDone:NO];
}

-(void) join:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent *)current
{
    SEL selector = @selector(join:name:);
    NSMethodSignature* sig = [[target class] instanceMethodSignatureForSelector:selector];
    NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:sig];
    [invocation setTarget:target];
    [invocation setSelector:selector];
    [invocation setArgument:&timestamp atIndex:2];
    [invocation setArgument:&name atIndex:3];
    [invocation retainArguments];
    
    [invocation performSelectorOnMainThread:@selector(invokeWithTarget:) withObject:target waitUntilDone:NO];
}

-(void) leave:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent *)current
{
    SEL selector = @selector(leave:name:);
    NSMethodSignature* sig = [[target class] instanceMethodSignatureForSelector:selector];
    NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:sig];
    [invocation setTarget:target];
    [invocation setSelector:selector];
    [invocation setArgument:&timestamp atIndex:2];
    [invocation setArgument:&name atIndex:3];
    [invocation retainArguments];
    
    [invocation performSelectorOnMainThread:@selector(invokeWithTarget:) withObject:target waitUntilDone:NO];
}

-(void)dealloc
{
    [target release];
    [super dealloc];
}
@end
