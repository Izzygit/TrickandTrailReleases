# Trick and Trail Setup Guide for Version 0.1
## Preface: 
My goal with this package is to provide the user the ability to tune the board in a more advanced way, but this comes with drawbacks. It is possible to make a Pitch tune that is not ridable. For instance, if you input max motor current (100A for the floatwheel) at .5 degree pitch angle it’s going to be way to sensitive. Similarly, if you tune 20A at .5 degree pitch and 100A at 1 degree, this slope will be too steep. 
The result will generally be board vibration. This is when the board bounces back and forth between over and under reacting at a fast rate. Not particularly dangerous but annoying. The real hurtle will be finding a Pitch tune you like with minimal vibration and then we can filter that vibration out. If you get past the Pitch tune section, all the other features should work well. 

## Loading
    1. Load up the VESC package. Download from the github folder and Load Custom in VESC Packages.
    2. Check the Safety Tiltback, Startup, Stop, and Specs and change the settings as needed. 
        a. In particular, check the High and Low Voltage Threshold and the Loop Hertz on the Specs tab to make sure they are compatible with your board. 
        
## Pitch Tune
    3. Try the default tune first. Surge, traction control and roll control are disabled by default for this version so you can set things up progressively.
    4. Mess around with the tune to get it where you like it in TNT Cfg -> Pitch. 
        a. The tune that is default only uses Kp0, Pitch 1, Pitch 1 Current, Pitch2, and Pitch 2 Current. Even though there are 6 points, most tunes can be defined with fewer points.
        b. I find that keeping Kp0 and Pitch 1 Current low is more like the XR’s mission.
        c. To make it stiffer close to the set point increase Kp0 and Pitch 1 Current. Pitch 1 Current will have more influence than Kp0.
        d. To make it looser close to the setpoint you can decrease Kp0 and Pitch 1 Current. If they are already low you can increase Pitch 1.
        e. To make less nose dip at high demand decrease Pitch 2 (better for trails).
        f. To loosen the tune at high demand increase Pitch 2 (better for tricks).
        g. If there is too much current increase over a short pitch distance, this will create board vibration. Smoother current demand curves are less likely to create vibration. More on tuning out vibration below.
    5. Test the pitch tune under higher load to see if there is vibration.  Something like uphill grass is good. 
        a. If there is vibration you can tune it out with the Kalman factors. You can observe smooth pitch compared to pitch on the APPUI to see the effect.
        b. Starting from Factor1= 5; Factor2= 0; Factor3=0.1. Increasing factor 3 will dampen the vibration response. When you start getting above 1 you will see the response starts to become delayed. To check the delay do a hard brake. There should be minimal "spring back" from the tail which is the braking overshooting.
        c. Once Factor3 is at 1 or above start to lower Factor1.
        d. Once Factor1 gets down to around 1 you will want to also reduce Factor3 so that they stay relatively close to each other or else the response can become delayed again.
        e. I have had success with (Factor1= 5; Factor2= 0; Factor3=1) and (Factor1= .5; Factor2= 0; Factor3=0.6)
    6. Once satisfied with the pitch tune, move on to the roll tune.
    
## Roll Tune
    7. For roll I recommend starting with these factors:
        a. Accelerating: kp1 = 0, roll1=5, kp2=0.5, roll2=15, kp3=0, roll3=0
        b. Braking: kp1 = 0, roll1=5, kp2=0.3, roll2=10, kp3=0, roll3=0
        c. Leave the low speed scaling as it is for now.
    8. Carve around to see how it feels. Downhill is the best to test braking roll. Increase and decrease the factors as you see fit. Observe active Roll Kp on the APPUI page.
    9. After you are satisfied with your roll tune, do some low speed circles to see how it feels before adjusting low speed scaling.
    10. I recommend keeping the ERPM values as 3000 and 750 for now. Increase the Maximum Scaler and do some low speed circles again to feel the effect. 500% will get a little intense. I use 350%.
    11. After you have found your low speed scaling, test it under a higher load for vibration. I recommend engaging your maximum acceleration roll kp while riding uphill grass at low speed. Adjust the Kalman factors on the pitch tune if necessary.
Surge
    1. Because surge is dependent on your pitch tune, different settings may be required. First enable surge.
    2. I recommend starting on grass for safety. To engage surge, quickly dip the nose. How quickly the nose comes up and for how long surge is engaged, depends on how much weight you commit to your front foot. 
    3. To make it easy to surge you can reduce the Start Angle Speed and the Start Differential. 
        a. If the Start Differential is too low you may engage surge when you don’t need it in bumpy terrain.
        b. If the Start Angle Speed is too low you will engage surge during normal board motions. 
        c. You will notice if the board surges too easily, it will click. That is it entering surge only for a split second and immediately exiting. We want to tune the start conditions so surge only starts when we need it. It should give us a good boost of power without these nuisance clicks. 
        d. Pay attention to the APPUI screen which will show you when the last surge occurred and how long it surged for. Less than 0.03s is a nuisance. A good surge is .1-.2s.
    4. If surge feels like it is overshooting you too much and you cannot commit enough weight to hold the nose down, you can reduce the Current Margin down to a minimum of 1. If you would like more surge you can increase this number.

## Traction Control
    1. Enable traction control
    2. First you will want to see if the settings are roughly in the right range for your board configuration. To do this, start on grass. Either surge and de-weight the board or brake hard and de-weight to get the tire to slip. Observe the debug information on APPUI.
    3. Switch traction control on and off to see that it is adequately reducing wheel slip.
    4. If traction control is causing the board to turn off, that means our settings to end traction control could be too strict. Increase Wheelslip Acceleration End and Wheelslip Margin so that Margin remains greater than Accel End. Tune these values until the board reliably ends traction control.
    5. If it seems like traction control is not reducing wheel slip enough you may decrease the Wheelslip Acceleration End and Wheelslip Margin but this is not likely to be a problem.
    6. If all is working well, you can now check the Wheelslip Acceleration Trigger. 
        a. You will see on APPUI debug, Wheelslip Start Accel, which indicates the acceleration that started wheel slip. 
        b. This Wheelslip Start Accel value is based on Wheelslip Acceleration Trigger, scaled at low speed by Wheelslip Low Erpm Scaler and Scale ERPM. These are the values you are looking to tune.
        c. On APPUI under debug you will see Start ERPM Factor which is the value that is multiplied by Wheelslip Acceleration Trigger based on Wheelslip Low Erpm Scaler and Scale ERP.
        d. Starting at various speeds, accelerate as fast as possible. If traction control engages when it shouldn’t, you will hear the motor click. 
        e. Look at your debug values and tune Wheelslip Acceleration Trigger, Wheelslip Low Erpm Scaler and Scale ERPM as needed so that traction control does not engage during normal acceleration. Default tune is based on 15s hypercore.
        f. If these values are tuned so that it is too hard to enter traction control you could end up with a delayed traction control reaction which is not safe to ride. I found that with a 15s hypercore I could hit an acceleration of 25, so I tuned my start condition for 35. If I were to tune it to 100 that would be dangerous. 
*Tip sometimes stricter front footpad settings are needed to keep the board from turning off from half switch while in traction control. I use 500ms and 200 ERPM.