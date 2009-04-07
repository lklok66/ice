#import <ChatRoomCallbackI.h>

@interface ChatRoomCallbackI()

@property (nonatomic, retain) ChatViewController* target;

@end

@implementation ChatRoomCallbackI

@synthesize target;

-(id)initWithTarget:(ChatViewController*)t
{
    if((self = [super init]))
    {
        self.target = t;
    }
    return self;
}

+(id)chatRoomCallbackWithTarget:(ChatViewController*)target
{
    return [[[ChatRoomCallbackI alloc] initWithTarget:target] autorelease];
}

-(void) init:(ICEMutableStringSeq *)users current:(ICECurrent *)current
{
    [target initUsers:users];
}

-(void) send:(ICELong)timestamp name:(NSMutableString *)name message:(NSMutableString *)message current:(ICECurrent *)current
{
    [target send:timestamp name:name message:message];
}

-(void) join:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent *)current
{
    [target join:timestamp name:name];
}

-(void) leave:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent *)current
{
    [target leave:timestamp name:name];
}

-(void)dealloc
{
    [target release];
    [super dealloc];
}
@end
