package com.example.wiiphone;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.drawable.ShapeDrawable;
import android.graphics.drawable.shapes.OvalShape;
import android.graphics.drawable.shapes.RectShape;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

public class JoystickHorizontalView extends View
{
	private static final int AXIS_X = 0;
	private ShapeDrawable mDrawableInner = null;
	private ShapeDrawable mDrawableOuter = null;
	private ShapeDrawable mDrawCircle = null;
	private int mXPos = 50;
	private int mYPos = 250;
	private int mWidth = 250;
	private int mHeight = 50;
	private int mSteeringCircle = 175;
	private int mCircleWidth = 75;
	
	public JoystickHorizontalView(Context context, AttributeSet attrs) 
	{
		super(context, attrs);
		
		mXPos = (int) 50;
		mYPos = (int) 250;
		mWidth = 250;
		mHeight = 75;
		super.setRight(mXPos + mWidth);
		super.setLeft(mWidth);
		super.setTop(mYPos);
		super.setBottom(mYPos + mHeight);
		
		mSteeringCircle = mXPos + (mWidth / 2) - (mCircleWidth / 2);
		System.out.println("xPos: " + mXPos + "yPos" + mYPos );
		System.out.println("x: " + mWidth + "y" + mHeight );
		mDrawableInner = new ShapeDrawable(new RectShape());
		mDrawableInner.getPaint().setARGB(255,255,0,0);
		mDrawableInner.setBounds(mXPos, mYPos, mXPos + mWidth, mYPos + mHeight);
		
		mDrawableOuter = new ShapeDrawable(new RectShape());
		mDrawableOuter.setBounds(mXPos, mYPos, mXPos + mWidth, mYPos + mHeight);
        mDrawableOuter.getPaint().setARGB(255,255,255,255);
        mDrawableOuter.getPaint().setStyle(Paint.Style.STROKE);
		mDrawableOuter.getPaint().setStrokeWidth(10);
		
		mDrawCircle = new ShapeDrawable(new OvalShape());
		mDrawCircle.getPaint().setARGB(255,0,0,255);
		mDrawCircle.setBounds(mSteeringCircle, mYPos , mSteeringCircle + mCircleWidth, mYPos + mCircleWidth);
	}
	@Override
	public boolean onTouchEvent(MotionEvent event) 
	{
		if(event.getAction() == MotionEvent.ACTION_MOVE || event.getAction() == MotionEvent.ACTION_DOWN)  
		{
			
			System.out.println("LOLHEHE");
			mSteeringCircle = (int) event.getAxisValue(AXIS_X);
			if(mSteeringCircle > (mXPos + mWidth) - (mCircleWidth / 2))
			{
				mSteeringCircle = mXPos + mWidth - (mCircleWidth / 2);
			}
			
			if(mSteeringCircle < (mXPos) - (mCircleWidth / 2))
			{
				mSteeringCircle = mXPos - (mCircleWidth / 2);
			}			
		}
		if(event.getAction() == MotionEvent.ACTION_UP)
		{
			mSteeringCircle = mXPos + (mWidth / 2) - (mCircleWidth / 2);
		}
		mDrawCircle.setBounds(mSteeringCircle, mYPos , mSteeringCircle + mCircleWidth, mYPos + mCircleWidth);
		postInvalidate();
		return true;
	} 
	@Override
	protected void onDraw(Canvas canvas) 
	{
		super.onDraw(canvas);
		
		if(mDrawableInner != null)
		{
			mDrawableInner.draw(canvas);
		}
		if(mDrawableOuter != null)
		{
			mDrawableOuter.draw(canvas);
		}
		if(mDrawCircle != null)
		{
			mDrawCircle.draw(canvas);
		}
	}
}
