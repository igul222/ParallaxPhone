//
//  SampleFacade.m
//  OpenCV Tutorial
//
//  Created by BloodAxe on 8/14/12.
//
//

#import "SampleFacade.h"
#import "NSString+StdString.h"
#import "UIImage2OpenCV.h"
#import <Foundation/Foundation.h>

@interface SampleFacade()
{
    NSString * m_title;
    NSString * m_description;
    UIImage  * m_smallIcon;
    UIImage  * m_largeIcon;
    
    SampleBase * _sample;
    
    AsyncUdpSocket *socket;
}

@end


@implementation SampleFacade
//@synthesize sample = _sample;

- (id) initWithSample:(SampleBase*) s
{
    assert(s);
    
    if (self = [super init])
    {
        _sample = s;
        socket = [[AsyncUdpSocket alloc] initWithDelegate:self];
    }
    
    return self;
}

- (NSString *) title
{
    if (!m_title)
    {
        m_title = [NSString stringWithCString:_sample->getName().c_str() encoding:NSASCIIStringEncoding];
    }
    
    return m_title;
}

- (NSString *) description
{
    if (!m_description)
    {
        m_description = [NSString stringWithCString:_sample->getDescription().c_str() encoding:NSASCIIStringEncoding];
    }
    
    return m_description;
}


- (UIImage*)   smallIcon
{
    if (!m_smallIcon)
    {
        if (_sample->hasIcon())
        {
            NSString * iconStr = [NSString stringWithStdString:_sample->getSampleIcon()];
            m_smallIcon = [[UIImage imageNamed:iconStr] thumbnailWithSize:80];
        }
        else
        {
            UIImage * srcImage = [UIImage imageNamed:@"DefaultSampleIcon.png"];
            m_smallIcon = [self processFrame:[srcImage thumbnailWithSize:80]];
        }
    }
    
    return m_smallIcon;
}

- (UIImage*)   largeIcon
{
    if (!m_largeIcon)
    {
        if (_sample->hasIcon())
        {
            NSString * iconStr = [NSString stringWithStdString:_sample->getSampleIcon()];
            m_largeIcon = [UIImage imageNamed:iconStr];
        }
        else
        {
            UIImage * src = [UIImage imageNamed:@"DefaultSampleIcon.png"];
            assert(src);

            m_largeIcon = [self processFrame:src];
        }
    }
    
    return m_largeIcon;
}

- (bool) processFrame:(const cv::Mat&) inputFrame into:(cv::Mat&) outputFrame
{
    bool result = _sample->processFrame(inputFrame, outputFrame);
    
    NSDictionary *dict = @{@"x": @(_sample->topX), @"y": @(_sample->topY)};
    NSData *data = [NSJSONSerialization dataWithJSONObject:dict options:0 error:nil];
    
    [socket sendData:data
              toHost:@"67.194.73.12"
                port:8888
         withTimeout:-1
                 tag:0];
    
    return result;
}

- (UIImage*) processFrame:(UIImage*) source
{
    cv::Mat inputImage = [source toMat];
    cv::Mat outputImage;
    
    _sample->processFrame(inputImage, outputImage);
    UIImage * result = [UIImage imageWithMat:outputImage andImageOrientation:[source imageOrientation]];
    return result;
}

-(void)onUdpSocket:(AsyncUdpSocket *)sock didSendDataWithTag:(long)tag {
}

- (NSString *) friendlyName
{
    return [NSString stringWithCString:_sample->getUserFriendlyName().c_str() encoding:NSASCIIStringEncoding];
}

- (bool) getIsReferenceFrameRequired
{
    return _sample->isReferenceFrameRequired();
}

- (void) setReferenceFrame:(cv::Mat&) referenceFrame
{
    _sample->setReferenceFrame(referenceFrame);
    
}

- (void) resetReferenceFrame
{
    _sample->resetReferenceFrame();
}

- (OptionsMap) getOptions
{
    return _sample->getOptions();
}

@end
