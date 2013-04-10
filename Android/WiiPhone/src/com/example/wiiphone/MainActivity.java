package com.example.wiiphone;

import android.app.Activity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.util.Log;
import android.widget.SeekBar;

@SuppressWarnings("deprecation")
public class MainActivity extends Activity implements SensorEventListener
{
    private TCPClient mTcpClient = null;
    private SensorManager mSensorManager = null;
    private Sensor mAccelerometer = null;
    private PowerManager mPowerManager = null;
    private WakeLock mWakeLock = null;
 
    
	@Override
    public void onCreate(Bundle savedInstanceState)
    {
    	System.out.println("onCreate START");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        // Get an instance of the SensorManager.
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        
        // Get the sensor (Accelerometer).
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        
        // Register Delays and Listeners.
        mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_GAME);
        
        // Get an instance of the PowerManager.
        mPowerManager = (PowerManager) getSystemService(POWER_SERVICE);
        
        mWakeLock = mPowerManager.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, getClass()
                .getName());
        
        // connect to the server.
        new connectTask().execute("");
        
        // Get the SeekerBar so we can add onChange event.
        VerticalSeekBar throttle = (VerticalSeekBar)findViewById(R.id.throttleSeekBar);
        
        // Set the OnBarChange method.
        throttle.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() 
        {
			@Override
			public void onStopTrackingTouch(SeekBar seekBar)
			{
				// No need to implement but has to override.
			}
			
			@Override
			public void onStartTrackingTouch(SeekBar seekBar)
			{
				// No need to implement but has to override.
			}
			
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) 
			{
				if (mTcpClient != null) {
					String message = "SPD " +  Float.toString(progress);
                    mTcpClient.sendMessage(message);
                    
                    Log.e("SPD MESSAGE", message);
                }
			}
		});
        System.out.println("onCreate END");
    }
    
    // Overriding all onX events for debugging cause I'm a noob at Android programming.
    @Override
    public void onResume()
    {
    	System.out.println("onResume START");
        super.onResume(); 
        mWakeLock.acquire();
        
        System.out.println("onResume END");
    }
    
    @Override
    public void onPause()
    {
    	System.out.println("onPause START");
        super.onPause();
        mTcpClient.stopClient(); // Stop update if we close app.
        mWakeLock.release();
        
        System.out.println("onPause END");
    }
    
    @Override
    public void onStart()
    {
    	System.out.println("onStart START");
        super.onStart();
        System.out.println("onStart END");
    }
    
    @Override
    public void onStop()
    {
    	System.out.println("onStop START");
        super.onStop();
        System.out.println("onStop END");
    }
    
    @Override
    public void onDestroy()
    {
    	System.out.println("onDestroy START");
        super.onDestroy();
        System.out.println("onDestroy END");
    }
    
    @Override
    public void onSensorChanged(SensorEvent event) 
    {
    	if (event.sensor.getType() != Sensor.TYPE_ACCELEROMETER)
            return;
    	
    	if (mTcpClient != null) 
    	{
    		String message = "ACC " + Float.toString(event.values[0]) + " "
    	   			 + Float.toString(event.values[1]) + " " + Float.toString(event.values[2]);
    		mTcpClient.sendMessage(message);
    		
    		Log.e("ACC MESSAGE", message);
    	}
    }
    
    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) 
    {
    	// Have to override this
    }
    
    public class connectTask extends AsyncTask<String,String,TCPClient> 
    {
    	
        @Override
        protected TCPClient doInBackground(String... message) 
        {
            //we create a TCPClient object and
            System.out.println("Connection to Server");
    		mTcpClient = new TCPClient(new TCPClient.OnMessageReceived() {
                @Override
                //here the messageReceived method is implemented
                public void messageReceived(String message) {
                    //this method calls the onProgressUpdate
                    publishProgress(message);
                }
            });
        	mTcpClient.run();
 
            return null;
        }
 
        @Override
        protected void onProgressUpdate(String... values) {
            super.onProgressUpdate(values);
             // Somehow this needs to be here.
        }
    }
}