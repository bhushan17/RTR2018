package com.matrix_android.hello_view_object;

//default given packeges
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;

//added by user
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;
import android.graphics.Color;

import androidx.appcompat.widget.AppCompatTextView;
import android.content.Context;
import android.view.Gravity;


public class MainActivity extends AppCompatActivity {

	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
		
		System.out.println("RTR: Inside OnCreate.");
		AppCompatTextView MyView = new AppCompatTextView(this);
		
		MyView.setTextColor(Color.rgb(255,128,0));
		MyView.setTextSize(60);
		MyView.setGravity(Gravity.CENTER);
		MyView.setText("Hello World !!!");
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
		//myView= new MyView(this);
		//set this view as our main view
		setContentView(MyView);
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



