#import <ChatSession.h>

@interface ChatRoomCallbackI : ChatChatRoomCallback
{
@private
    id target;
}
-(id)initWithTarget:(id)target;
+(id)chatRoomCallbackWithTarget:(id)target;
-(void) init:(ICEMutableStringSeq *)users current:(ICECurrent *)current;
-(void) send:(ICELong)timestamp name:(NSMutableString *)name message:(NSMutableString *)message current:(ICECurrent *)current;
-(void) join:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent *)current;
-(void) leave:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent *)current;

@end
