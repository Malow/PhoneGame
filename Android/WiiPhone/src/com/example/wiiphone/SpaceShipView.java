package com.example.wiiphone;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.drawable.ShapeDrawable;
import android.graphics.drawable.shapes.RectShape;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

public class SpaceShipView extends View
{
	private TCPClient mTcpClient = null;
	private ShapeDrawable mDrawable = null;
	
	public static final int AXIS_Y = 1;
	
    public SpaceShipView(Context context, AttributeSet attrs) 
    {
        super(context, attrs);
        
        mTcpClient = null;
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
                
                int x = 0;
                int y =  (int) (super.getHeight() * (1 - (progress / 100)));
                int width = 300;
                int height = (int) (super.getHeight() * (progress / 100));

                mDrawable = new ShapeDrawable(new RectShape());
                mDrawable.getPaint().setARGB(255,255,0,0);
                mDrawable.setBounds(x, y, x + width, y + height);
                postInvalidate();
            }
        }
        return true;
    }
    
    @Override
    protected void onDraw(Canvas canvas) 
    {
    	if(mDrawable != null)
    	{
    		mDrawable.draw(canvas);
    	}
    }
    
    public void setTcpClient( TCPClient tcp )
    {
    	mTcpClient = tcp;
    }
}