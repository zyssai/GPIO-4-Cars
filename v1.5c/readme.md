## Here is a tutorial for GPIO'4'Cars boards revision **1.5b** and newer, with latest [LINEAGE OS 20](https://forum.odroid.com/viewtopic.php?f=178&t=45758), please just use non-vulkan for the moment, because there is some problem while using gpio with vulkan version.

Should work with other OS's but not tested.

**Device must be rooted**, follow [this tutorial](https://forum.odroid.com/viewtopic.php?f=178&t=34063).

## **VU7A+ screen users:**

To control brightness of the screen, you need to solder a wire from your screen [as shown here](https://forum.odroid.com/viewtopic.php?p=323833#p323833), to PWM pin of the GPIO'4'Cars AUX connector (tested with LOS18, and LOS20). No need to add any resistor, it is included in the board.  
[Here](https://wiki.odroid.com/accessory/display/vu_series/vu7a_plus/vu7a_plus#tab__odroid-n2c41) is how to setup correct resolution and display.  

To have a working touchscreen with LOS20, you need to add `extra_boot_args="usbhid.quirks=0x16b4:0x0705:0x0004"` to the end of `/odm/env.ini`.  
Don't forget `pwm_cd pwm_backlight` into *DTB Overlay* section of *Odroid Settings* app.

## VU7C screen users:

You can plug VU7C screen directly on top of GPIO'4'Cars board, or you can use a 40pin ribbon cable to connect your VU7C screen. One male side, and the other female. If you order the board, I can sell you one with the desired length, but there is some bad behavior when using too long ribbon cable, so be careful.

DO NOT USE 80 wires ribbon cable!!! You will damage your N2, as the additional wires are grounded with IDE standard, and N2 is not following this standard. 

![](https://i.imgur.com/H2YJgyo.jpg)

Please be careful while plug it in, match pin1 of screen to pin1 of the GPIO'4'Cars connector.
Wrong connection can damage the screen and/or your N2.

Before connecting the screen, launch Odroid Settings app, and in DTB Overlay section, enter this: `display_vu7c`.

With this overlay, sound output is used by HDMI port. So if you need to have sound output to N2 USB or analog, do the following:

Download [display_vu7c_no_dai.dtbo](https://github.com/zyssai/GPIO-4-Cars/blob/main/display_vu7c_no_dai.dtbo) and place it to `/odm` folder.

Edit `/odm/env.ini` and add `display_vu7c_no_dai` to `user_overlays` at the end of the file.

Launch Odroid Settings app, and in DTB Overlay section, enter this: `display_vu7c_no_dai`.

Shutdown, connect the screen and try.

## Steering wheel controller connector:

If you want to connect 8 switches or less, just wire one side of each switch to GND, and the other side of each switch to SWC pin, like this.

![](https://imgur.com/ai4GVdy.jpg)

This is physical pin numbers. Please see [here](https://wiki.odroid.com/odroid-n2/application_note/gpio/wiringpi) correspondence with wiringpi numbers.

Note that pin 26 is labelled with a square on the board.

![](https://imgur.com/Hq4ODG2.jpg)

Now, go down to [How to manage AUX and POWER connectors with Android](https://github.com/zyssai/GPIO-4-Cars/blob/main/v1.5c/readme.md#how-to-manage-aux-and-power-connectors-with-android) chapter below and follow the same process to define every button to desired action.  

If you intend to use pin 32 with LOS20, state of this pin at start is defined wrong, so follow these steps:  

Edit `/vendor/etc/init/hw/init.odroidn2.rc`  
Find `on init`  
Add `start gpio26up` like this, respect spaces:

```
on init
  start gpio26up
```

Then go at the end of the file, and add this:

```
service gpio26up /system/bin/gpio mode 26 up
  class main
  disabled
  oneshot
```

Save, restart.  

If you need more than 8 switches (up to 16), you need to use keymatrix functionality.

## Steering wheel controller connector using keymatrix functionality:

Here is how to deal with it.
Wire your switches like this (4 rows and 4 columns):

![](https://imgur.com/GWD4PyH.jpg)

Now you have 8 wires, connect them all to SWC connector.  

Here is correspondence of pins.

![](https://imgur.com/gXVDWUq.jpg)

Modify [keymatrix.dts](https://github.com/zyssai/GPIO-4-Cars/blob/main/v1.5c/keymatrix.dts) file according to your needs, but this generic file should work.

Compile it to generate dtbo file by following [these steps](https://github.com/zyssai/GPIO-4-Cars/blob/main/howto/Howto_compile_DTS_overlay), or if you want to use generic file, you can just use this [keymatrix.dtbo](https://github.com/zyssai/GPIO-4-Cars/blob/main/v1.5c/keymatrix.dtbo) file.

Place `keymatrix.dtbo` to the `/odm` folder.  

Edit `/odm/env.ini`, go down to find `user_overlay` and add `keymatrix` at the end.

```
user_overlay="keymatrix"
```

And add `keymatrix` to the `DTB Overlay` section of `Odroid Settings` app.

If you use LOS20, state of pin 32 is defined wrong at start, you can check it with:

```
su
cat /sys/kernel/debug/gpio
```

if there is one of the `matrix_kbd_***` at `lo`, then do the following steps:

Edit `/vendor/etc/init/hw/init.odroidn2.rc`  
Find `on init`  
Add `start gpio26up` like this, respect spaces:

```
on init
  start gpio26up
```

Then go at the end of the file, and add this:

```
service gpio26up /system/bin/gpio mode 26 up
  class main
  disabled
  oneshot
```

Save, restart.

Then to manage your buttons actions, use the free and perfect [KeyMapper](https://play.google.com/store/apps/details?id=io.github.sds100.keymapper) app.  

Add all actions, it's easy to understand.

## **How to manage AUX and POWER connectors with Android:**

First step is to declare which pin numbers are used.  
This is done by using [Shutdownservice](https://forum.odroid.com/viewtopic.php?f=213&t=44791) app from joerg (Yes this is a post from Odroid M1 section but it's ok)  
Last version tested and working is `1.15.2`  
Install it, and give superuser rights.  

Before using this app, we need to edit these 2 files:  
`/vendor/etc/public.libraries.txt`  
`/system/etc/public.libraries.txt`  

Add this to the end of each:  
`libwiringPi.so`  

Reboot, launch the app.  
If it forces closes, open a terminal, and type this:

```
odroidn2:/ $ su
odroidn2:/ # mount -o remount,rw /
odroidn2:/ # cp  /vendor/lib64/libwiringPi.so /system/lib64/libwiringPi.so
```

Reboot, launch the app, clear all existing values, and setup like this:

```
Shutdown delay: 3000
On Start delay: 1000
Extra Pin1: 15
Extra Pin2: -16 (yes, there is a minus)
Extra Pin3: 24
Extra Pin4: 3
Extra Pin5: 0
```

Untick all `Update periodically`

All these pin numbers are wiringpi numbers, more information [here](https://wiki.odroid.com/odroid-n2/application_note/gpio/wiringpi).

Here is correspondence between physical and wiringpi pin values:

![](https://imgur.com/QpCcZzL.jpg)

Now this app will send intents to Android. You can catch them with app of your choice, Tasker or similar.  
I am personally using [Macrodroid](https://play.google.com/store/apps/details?id=com.arlosoft.macrodroid) app, as I find the interface very user-friendly. Since some time, this app is only 7 day trial. You need to buy Pro version, or use another one.  
Once it is installed, don't forget to enable root and experimental functionalities in app settings.  

Then download [this file](https://github.com/zyssai/GPIO-4-Cars/blob/main/v1.5c/MacroDroid_v1.5c_24_03_16.mdr) containing all macros, and import it into Macrodroid.

For example with `Ignition` event, you can setup an OFF and an ON action.

![](https://imgur.com/1O2icj3.jpg)

Use action of your choice, here I reduce brightness and launch clock screensaver when I turn ignition OFF.

![](https://imgur.com/FWNJtkE.jpg)

Set intent like this:

![](https://imgur.com/yPfoer9.jpg)

![](https://imgur.com/ACndEmA.jpg)

**gpio** intent is the wiringpi pin number, and **value** is the state that will launch macro.  
So **1** is OFF state, and **0** is ON state. It is reverted due to the wiring of the board.  

**Shutdown macro is here to power off Android properly, before GPIO'4'Cars board cut power off. So macro must include a shutdown process.  
If Android is still running after around 1 minute, board will cut power to prevent car battery drain.**

## How to use INPUT and OUTPUT pin:

Input will be triggered when there is incoming 12V at `AUX_IN` pin.
So you just have to edit `AUX_INPUT` macro and add your actions (launch reverse camera app if you wire a reverse gear signal, set low brightness if you wire your light signal, and so on...)

Output pin can be triggered with a Macrodroid widget.
Add the Output widget to home.
Then when you click on the widget, 12v will be available on `AUX_OUTPUT` pin. You can connect load of around 3A maximum.
Output pin will always be low when Android is off.
You can also trigger output macro from another macro. For example, wire your remote amplifier signal to `AUX_OUTPUT`.
Trigger output macro with ignition macro.
When you start your engine, your amplifier is powering on.
