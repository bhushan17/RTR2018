
//
// AppDelegate.m
// Window
//

#import "AppDelegate.h"
#import "ViewController.h"
#import "MyView.h"

@implementation AppDelegate
{
@private
    UIWindow *mainWindow;
    ViewController *mainViewController;
    MyView *myView;
    
}

-(bool)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    //get screen bounds for fullscreen
    CGRect screenBounds=[[UIScreen mainScreen]bounds];
    
    //intialize window variable corresponding to screen bounds
    mainWindow=[[UIWindow alloc]initWithFrame:screenBounds];
    
    mainViewController=[[ViewController alloc]init];
    
    [mainWindow setRootViewController:mainViewController];
    
    //intialize view variable corresponding to screen bounds
    myView=[[MyView alloc]initWithFrame:screenBounds];
    
    [mainViewController setView:myView];
    
    [myView release];
    
    //add the ViewController's view as subview to the window
    [mainWindow makeKeyAndVisible];
    return(YES);
}

-(void)applicationWillResignActive:(UIApplication *)application
{
    /*
    Sent when the application is about to move from active to inactive state.
    This can occur for certain types of temporary interrution
    (such as an incoming phone call or SMS nmessages)or when the user quits
    the application and it begins the transition to the background state.
    Use this methid to pause ongoing task, disable timers, and invalidate
    graphics rendering callback. games should use method to pause the
    game.
    */
    
}

-(void)applicationDidEnterBackground:(UIApplication *)application
{
    /*
    use this method to release shared resources, save user data, invalidate
    timers, and store enough application state information to restore your
    application to its current state in case it is terminated later.
    If your application supports background execution, this method is called
    instated of applicationWillTerminate: when the user quits.
    */
}

-(void)applicationWillEnterForeground:(UIApplication *)application
{
    //Called as part of the transition from the background to the active state;
    //here you can undo many of the changes made on entering the background.
    
}

-(void)applicationDidBecomeActive:(UIApplication *)application
{
    /*
    Restart any task that were paused (or not yet started) while the
    application was inactive. IF the application was previously in the
    background, optionally refresh the user interface.
    */
}

-(void)applicationWillTerminate:(UIApplication *)application
{

    //called when the application is about to terminate. Save data if
    //appropriate. See also applicationDidEnterBackground:.
}

-(void)dealloc
{
    //code
    [myView release];
    
    [mainViewController release];
    
    [mainWindow release];
    
    [super dealloc];
}

@end





