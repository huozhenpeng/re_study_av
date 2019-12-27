package com.miduo.restudyav.Widget;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;

import java.util.ArrayList;
import java.util.List;

public class DbLineView extends View {
    public DbLineView(Context context) {
        this(context,null);
    }

    public DbLineView(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs,-1);
    }

    public DbLineView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }
    private int width;
    private int height;

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        width=w;
        height=h;
    }

    private void init()
    {
        paint=new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setColor(Color.BLUE);
    }

    //总时间
    private int totalTime;
    public void setTotalTime(int totalTime)
    {
        //Log.e("abc","total--->"+totalTime);
        this.totalTime=totalTime;
    }

    List<PointF> datas=new ArrayList<>();

    //正常分贝在0到100之间，一般在60左右
    //假设最高就是100，最低是0
    //为了好看让db减去50
    public  void setCurrentDb(float currentTime,int db)
    {
        //Log.e("abc","currentTime--->"+currentTime+"+++++++++++"+db);
        PointF pointF=new PointF();
        pointF.x=currentTime*width/totalTime;
        float y=db*height/100;
        pointF.y=height-y;
        datas.add(pointF);
        invalidate();
    }
    private Paint paint;

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        canvas.drawColor(Color.WHITE);
        for(int i=0;i<datas.size();i++)
        {
            PointF pointF=datas.get(i);
            canvas.drawLine(pointF.x,height,pointF.x,pointF.y,paint);
        }
    }


}
