//
// MyView.m
//Window
//
//

#import "MyView.h"

@implementation MyView
{
	NSString *centralText;
}

-(id)initWithFrame:(CGRect)frameRect
{

	self=[super initWithFrame:frameRect];
	if(self)
	{
		//Intialization code here.
		
		//set scen's background color
		[self setBackgroundColor:[UIColor whiteColor]];
		
		centralText=@"Hello World !!!";
		
		//GESTURE RECOGNITION
		//Tap gesture code
		UITapGestureRecognizer *singleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
		[singleTapGestureRecognizer setNumberOfTapsRequired:1];
		[singleTapGestureRecognizer setNumberOfTouchesRequired:1]; //touch of 1 finger
		[singleTapGestureRecognizer setDelegate:self];
		[self addGestureRecognizer:singleTapGestureRecognizer];
		
		UITapGestureRecognizer *doubleTapGestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
		[doubleTapGestureRecognizer setNumberOfTapsRequired:2];
		[doubleTapGestureRecognizer setNumberOfTouchesRequired:1];//touch of 1 finger
		[doubleTapGestureRecognizer setDelegate:self];
		[self addGestureRecognizer:doubleTapGestureRecognizer];
		
		//this will allow to differentiate between single tap and double tap
		[singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
		
		//swipe gesture
		UISwipeGestureRecognizer *swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
		[self addGestureRecognizer:swipeGestureRecognizer];
		
		//long-press gesture
		UILongPressGestureRecognizer *longPressGestureRecognizer=[[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
		[self addGestureRecognizer:longPressGestureRecognizer];
	}
	return(self);
}


//Only override drawRect:if youperform custom drawing.
//An empty implementation adversely affects performance during animation
-(void)drawRect:(CGRect)rect
{
    //black background
    UIColor *fillColor=[UIColor blackColor];
    [fillColor set];
    UIRectFill(rect);
    
    //dictionary with kvc
    NSDictionary *dictionaryForTextAttributes=[NSDictionary dictionaryWithObjectsAndKeys:[UIFont fontWithName:@"Helvetica" size:24],NSFontAttributeName,[UIColor greenColor],NSForegroundColorAttributeName,nil];
    
    CGSize textSize=[centralText sizeWithAttributes:dictionaryForTextAttributes];
    
    CGPoint point;
    point.x=(rect.size.width/2)-(textSize.width/2);
    point.y=(rect.size.height/2)-(textSize.height/2)+12;
    
    [centralText drawAtPoint:point withAttributes:dictionaryForTextAttributes];
    
}








// to become first responder
-(BOOL)acceptsFirstResponder
{
	//code
	return(YES);
	
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{

	//code
	
	/*
	centeralText = @"'touchesBegan' Event Ocuured";
	[self setNeedDisplay]; //repainting
	*/
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr
{

	//code
	centralText=@"'onSingleTap' Event Occured";
	[self setNeedsDisplay]; //repainting
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{

	//code
	centralText=@"'onDoubleTap' Event Occured";
	[self setNeedsDisplay]; //repainting
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{

	//code
	[self release];
	exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr
{
	//code
	centralText=@"'onLongPress' Event Occured";
	[self setNeedsDisplay]; //repainting
	
}

-(void)dealloc
{

	//code
	[super dealloc];
}

@end



	
		
		
		
		
