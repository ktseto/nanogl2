/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.gl2jni;

//import android.app.Activity;
//import android.os.Bundle;
//import android.util.Log;
//import android.view.WindowManager;
//
//import java.io.File;
//
//
//public class GL2JNIActivity extends Activity {
//
//    GL2JNIView mView;
//
//    @Override protected void onCreate(Bundle icicle) {
//        super.onCreate(icicle);
//        mView = new GL2JNIView(getApplication());
//	setContentView(mView);
//    }
//
//    @Override protected void onPause() {
//        super.onPause();
//        mView.onPause();
//    }
//
//    @Override protected void onResume() {
//        super.onResume();
//        mView.onResume();
//    }
//}


//import android.support.v7.app.AppCompatActivity;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.SeekBar;
import android.widget.TextView;

import com.nineoldandroids.animation.ArgbEvaluator;
import com.nineoldandroids.animation.ObjectAnimator;
import com.nineoldandroids.animation.ValueAnimator;

import static android.view.View.VISIBLE;
import static android.view.animation.AnimationUtils.loadAnimation;

public class GL2JNIActivity extends Activity implements SeekBar.OnSeekBarChangeListener {

    public TextView textField, aboutMe;
    public TextView additionalText;
    public View glView;
    private int lastProgressValue = -1, sumOfProgresses;
    private int state = 0;
    public SeekBar rotation;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        rotation = (SeekBar)findViewById(R.id.rotation);
        rotation.setOnSeekBarChangeListener(this);

        additionalText = (TextView) findViewById(R.id.additional_text);
        textField = (TextView) findViewById(R.id.text);
        aboutMe = (TextView) findViewById(R.id.about_me);
        glView = findViewById(R.id.gl_view);

        introFadeIn();

        glView.setVisibility(VISIBLE);  // just for animation
    }

    private void introFadeIn() {
        final View[] views = {aboutMe, textField, rotation};
        for (int i = 0; i < views.length; i++ ){
            final View view = views[i];
            view.postDelayed(new Runnable() {
                @Override
                public void run() {
                    final Animation fadeIn2500 = AnimationUtils.loadAnimation(GL2JNIActivity.this, R.anim.in_alpha_2500);
                    view.setVisibility(VISIBLE);
                    view.startAnimation(fadeIn2500);
                }
            }, 1000 * i);
        }
    }

    @Override public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        switch (seekBar.getId()){
            case R.id.rotation:
                if (lastProgressValue == -1) { // first time when we touch slider we shouldn't think
                    lastProgressValue = progress;   // like we scrolled over a half of a slider
                    return;
                }
                sumOfProgresses += Math.abs(lastProgressValue - progress);
                lastProgressValue = progress;
                GL2JNILib.setRotation(((float) progress) / 1000.f);

                if (sumOfProgresses >= seekBar.getMax() && state != -1){
                    sumOfProgresses = 0;
                    state++;
                    onUpdateState(state);
                }
                break;
        }
    }

    private void onUpdateState(int state) {
        switch (state){
            case 1:
                textField.setText("State 1");
                blink(textField);
                break;
            case 2:
                textField.setText("State 2");
                blink(textField);
                break;
            case 3:
                textField.setText("State 3");
                blink(textField);
                break;
            case 4:
                textField.setText("State 4");
                blink(textField);
                break;
            case 5:
                textField.setText("State 5");
                blink(textField);
                break;
            case 6:
                aboutMe.setVisibility(View.GONE);
                textField.setVisibility(View.GONE);
                rotation.setVisibility(View.GONE);

                additionalText.setVisibility(VISIBLE);
                additionalText.startAnimation(loadAnimation(this, R.anim.in_from_bottom));
                break;
        }
    }

    private int currentTextColor = -1;
    private void blink(TextView view){
        if (currentTextColor == -1) currentTextColor = view.getCurrentTextColor();
        ValueAnimator colorAnim = ObjectAnimator.ofInt(view, "textColor", currentTextColor, 0xffff0000);
        colorAnim.setDuration(300);
        colorAnim.setEvaluator(new ArgbEvaluator());
        colorAnim.setRepeatCount(1);
        colorAnim.setRepeatMode(ValueAnimator.REVERSE);
        colorAnim.start();
    }

    @Override public void onStartTrackingTouch(SeekBar seekBar) {}
    @Override public void onStopTrackingTouch(SeekBar seekBar) {}
}
