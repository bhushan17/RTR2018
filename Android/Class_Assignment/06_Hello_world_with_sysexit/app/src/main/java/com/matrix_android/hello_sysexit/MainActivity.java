package com.matrix_android.hello_sysexit;

//default given packeges
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;

//added by user
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;
import android.graphics.Color;

public class MainActivity extends AppCompatActivity {

	private MyView myView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
		
		System.out.println("RTR: Inside OnCreate.");
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_main);
		
		//get Read of the title bar
		this.supportRequestWindowFeature(Window.FEATURE_NO_TITLE);
		//Remove navigation bar
		//View decorView = getWindow().getDecorView();
		//decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
		this.getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE
                                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                                    );
		//FullScreen
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		// do forced landscape
		this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		//set Background color
		this.getWindow().getDecorView().setBackgroundColor(Color.BLACK);
		//define our own View
		myView= new MyView(this);
		//set this view as our main view
		setContentView(myView);
    }
	
	@Override
	protected void onPause()
	{
		super.onPause();
	}
	
	@Override
	protected void onResume()
	{
		super.onResume();
	}
}



