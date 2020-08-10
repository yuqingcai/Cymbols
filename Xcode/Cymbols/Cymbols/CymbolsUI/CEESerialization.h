//
//  CEESerialization.h
//  Cymbols
//
//  Created by caiyuqing on 2019/7/18.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#ifndef CEESerialization_h
#define CEESerialization_h

@protocol CEESerialization <NSObject>
- (NSString*)serialize;
- (void)deserialize:(NSDictionary*)dict;
@end

#endif /* CEESerialization_h */
