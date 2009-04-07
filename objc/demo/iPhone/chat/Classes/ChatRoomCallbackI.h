#import <ChatSession.h>
#import <ChatViewController.h>

@interface ChatRoomCallbackI : ChatChatRoomCallback
{
@private
    ChatViewController* target;
}
-(id)initWithTarget:(ChatViewController*)target;
+(id)chatRoomCallbackWithTarget:(ChatViewController*)target;
-(void) init:(ICEMutableStringSeq *)users current:(ICECurrent *)current;
-(void) send:(ICELong)timestamp name:(NSMutableString *)name message:(NSMutableString *)message current:(ICECurrent *)current;
-(void) join:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent *)current;
-(void) leave:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent *)current;

@end
