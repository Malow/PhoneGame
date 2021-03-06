package com.example.wiiphone;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.drawable.ShapeDrawable;
import android.graphics.drawable.shapes.OvalShape;
import android.graphics.drawable.shapes.RectShape;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;


public class HelicopterView extends View
{
	private int mSwingMultiplier = 20;
	private TCPClient mTcpClient = null;
	private ShapeDrawable mDrawableOuter = null;
	private ShapeDrawable mDrawableInner = null;
	private ShapeDrawable mDrawableInnerPower = null;
	private ShapeDrawable mDrawableOuterPower = null;
	private ShapeDrawable mDrawableOuterBalanced = null;
	
	public static final int AXIS_Y = 1;
	
    public HelicopterView(Context context, AttributeSet attrs) 
    {
        super(context, attrs);
                
        int x = 40;
        int y =  (int) 320;
        int width = 170;
        int height = (int) 60;
        
        mDrawableOuterBalanced = new ShapeDrawable(new RectShape());
        mDrawableOuterBalanced.getPaint().setARGB(255,0,0,255);
        mDrawableOuterBalanced.getPaint().setStyle(Paint.Style.STROKE);
        mDrawableOuterBalanced.setBounds(x, y, x + width, y + height);
        mDrawableOuterBalanced.getPaint().setStrokeWidth(10);
        
        mTcpClient = null;
    }
    public void InvalidateView(float x, float y, float z)
    {
    	int mMiddleX = super.getWidth() / 2;
    	int mMiddleY = super.getHeight() / 2;
    	postInvalidate();
    	
    	int width = 200;
    	int height = 200;
    	
    	int lY = (int)x * mSwingMultiplier + mMiddleY - (height / 2);
    	int lX = (int)y * mSwingMultiplier + mMiddleX - (width / 2);
    	
    	mDrawableInner = new ShapeDrawable(new OvalShape());
    	mDrawableInner.getPaint().setARGB(255,255,0,0);
    	mDrawableInner.setBounds(lX, lY, lX + width, lY + height);
    	
    	mDrawableOuter = new ShapeDrawable(new OvalShape());
    	mDrawableOuter.getPaint().setARGB(255,0,255,0);
    	mDrawableOuter.getPaint().setStyle(Paint.Style.STROKE);
    	mDrawableOuter.getPaint().setStrokeWidth(10);
    	mDrawableOuter.setBounds(mMiddleX - (width / 2)-5, mMiddleY - (height / 2)-5,
    			mMiddleX - (width / 2) + width+5, mMiddleY - (height / 2) + height+5);
    	
        postInvalidate();
    }
    
    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) 
    {
        super.onSizeChanged(h, w, oldh, oldw);
    }
    
    @Override
    public boolean onTouchEvent(MotionEvent event) 
    {
        if(event.getAction() == MotionEvent.ACTION_MOVE || event.getAction() == MotionEvent.ACTION_DOWN
        		|| event.getAction() == MotionEvent.ACTION_UP)  
        {
			float progress = event.getAxisValue(AXIS_Y);
			progress = (progress / super.getHeight()); 
			progress = ((1 - progress) * 100);
			if (mTcpClient != null) 
			{
				if(progress > 100)
				{
					progress = 100.0f;
				}
				else if(progress < 0)
				{
					progress = 0.0f;
				}
				String message = "SPD " +  Float.toString(progress);
                mTcpClient.sendMessage(message);
                
                Log.e("SPD MESSAGE", message);
                
                int x = 50;
                int y =  (int) (super.getHeight() * (1 - (progress / 100)));
                int width = 150;
                int height = (int) (super.getHeight() * (progress / 100));

                mDrawableInnerPower = new ShapeDrawable(new RectShape());
                mDrawableInnerPower.getPaint().setARGB(255,255,0,0);
                mDrawableInnerPower.setBounds(x, y, x + width, y + height);
                
                
                mDrawableOuterPower = new ShapeDrawable(new RectShape());
                mDrawableOuterPower.getPaint().setARGB(255,0,255,0);
                mDrawableOuterPower.getPaint().setStyle(Paint.Style.STROKE);
            	mDrawableOuterPower.getPaint().setStrokeWidth(10);
                mDrawableOuterPower.setBounds(x, y, x + width, y + height);
                postInvalidate();
            }
			else
			{
				Log.e("ERROR SSView", "TCP === null");
			}
        }
        return true;
    }
    
    @Override
    protected void onDraw(Canvas canvas) 
    {
		if(mDrawableInner != null)
		{
			mDrawableInner.draw(canvas);
    	}
		if(mDrawableOuter != null)
    	{
    		mDrawableOuter.draw(canvas);
    	}
		if(mDrawableInnerPower != null)
    	{
    		mDrawableInnerPower.draw(canvas);
    	}
    	if(mDrawableOuterPower != null)
    	{
    		mDrawableOuterPower.draw(canvas);
    	}
    	if(mDrawableOuterBalanced != null)
    	{
    		mDrawableOuterBalanced.draw(canvas);
    	}
    }
    
    public void setTcpClient( TCPClient tcp )
    {
    	mTcpClient = tcp;
    }
}
