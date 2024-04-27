# Here is a tutorial for GPIO'4'Cars boards revision **1.5b** and newer, explaining how to install new OS from scratch, and prepare it for GPIO'4'Cars.

## Disclaimer

Follow instructions.

Stop if something is not working and solve it before going further.

I'm not reponsible for any problem, bad behavior, damage, explosion, bad coffee taste, cat attack or everything else.

## Download required files

Download and install [Balena Etcher](https://www.balena.io/etcher/) on a computer:

Download and install [Minimal ADB](https://androidmtk.com/download-minimal-adb-and-fastboot-tool) on Windows machine (not required on Linux):

Download last available [LineageOS 20](https://oph.mdrjr.net/voodik/S922X/ODROID-N2/Android/LineageOTA-20.0/builds/selfinstall/) (tested with alpha 0.2).
Do not use Vulkan version, there is problems with GPIO pins.
Do not unzip.

Download files from [this folder](https://github.com/zyssai/GPIO-4-Cars/tree/main/Save), put them on a USB key.

## OS installation

Put SPI switch selector of the N2 to SPI position.
Insert SD card, or emmc module.
Connect HDMI cable on a monitor (not VU7C screen for the moment), connect a keyboard, and power it on by connecting DC cable.

You will end to a menu. If nothing appears on the screen, try another monitor as the resolution is possibly unsupported by it.

Do `EXIT TO SHELL`
Then enter:

`ums /dev/mmcblk0`

Your N2 will now be recognized like a USB key, so connect it to your computer with a micro USB OTG port (near SPI switch).

Launch Balena Etcher, click on `FLASH FROM FILE`
Select previously downloaded LOS20 file, then `SELECT TARGET`
Your N2 must appear, select it.
Click on `FLASH!`

Once finished, type on N2:

```
ums
exit
```

It will going back to menu. There’s no way to shut it down so disconnect N2 DC cable.
disconnect micro USB cable.
Put SPI switch to MMC position.
Reconnect N2 DC cable.
Install will finish, and will end to Android.

If screen get stuck to Hardkernel logo, with blue LED always ON, restart with switch on SPI position. Do the same thing (ums /dev/mmcblk0, connect micro USB cable), then go to a file explorer, edit `env.ini` file.
Find `hdmi_autodetect`, and change to `false`.

## Required apps installation

Once you’re in Android, connect to internet (take care of date and time, must be correct to have a working connection).

Add your Google Account to Play Store.
If you are facing ID framework error, follow [this guide](https://www.google.com/android/uncertified/), obtain Google ID by launching Odroid Settings app, then Misc settings, and GSF ID.

From Play Store, install:

[Keymapper](https://play.google.com/store/apps/details?id=io.github.sds100.keymapper&pcampaignid=web_share)


[Macrodroid](https://play.google.com/store/apps/details?id=com.arlosoft.macrodroid&pcampaignid=web_share) (Better to buy Pro version or you will be limited, or need to find a similar app)

Install [Xplore](https://play.google.com/store/apps/details?id=com.lonelycatgames.Xplore&pcampaignid=web_share) application, or any other file manager with root capabilities.
Open it and gives file authorizations.
Go to Configuration, and change Root access to Superuser + mount writable.

Connect your USB key, then install all the following apps:

[GPIOShutdownservice](https://forum.odroid.com/viewtopic.php?f=213&t=44791) (tested working with 1.15.2)



[Macrodroid helper](https://www.macrodroidforum.com/index.php?threads/macrodroid-helper-apk.1/)



[Magisk](https://github.com/topjohnwu/Magisk/releases)

## Root and boot logo changes

First, into Odroid Settings app, go to Update, Updater, click on the 3 dots, preferences, and disable Auto updates check.
This will prevent auto updates because if there is, this will remove root and cause GPIO to not function anymore.

To root system, follow this:
Connect N2 with micro USB cable to the computer.
Launch a command terminal.
type:

`adb shell`

You should out with this:

`odroidn2:/ #`

Then type:

```
cd /storage/emulated/0/Download
dd if=/dev/block/boot of=boot.img
```

Now open Magisk app on N2, click `INSTALL` (the first row with Magisk), then `SELECT AND PATCH A FILE`

Click the 3 dots at the top left, then `ODROID-N2PLUS`, `DOWNLOAD`, and `BOOT.IMG`

Select `LET’S GO`

Once `ALL DONE`, take care of the output file name, should be something like `magisk_patched-xxxxx_xxxx.img`

Go back to the terminal, type (replace xxxxx):

```
dd of=/dev/block/boot
if=magisk_patched-xxxxx_xxxx.img
```

If it worked, you will have this:

```
odroidn2:/storage/emulated/0/Download # dd of=/dev/block/boot if=magisk_patched-25100_1NLE9.img
32768+0 records in
32768+0 records out
16777216 bytes (16 M) copied, 1.104671 s, 14 M/s
odroidn2:/storage/emulated/0/Download #
```

Now if you wanna change boot screens:

Copy your preferred boot image file and boot animation into Download folder:


`boot-logo.bmp`


`bootanimation.zip`

Then do the following:

```
remount
cp /storage/emulated/0/Download/boot-logo.bmp /odm/
chmod 755 /odm/boot-logo.bmp
cp /storage/emulated/0/Download/bootanimation.zip /system/product/media/
chmod 755 /system/product/media/bootanimation.zip
```

You can close Magisk, terminal, disconnect micro USB cable and reboot.
Open again Magisk, it should ask you to end installation.
You can also open Magisk again, then go to settings, and set `Superuser notification` to `None`, this will prevent annoying notification each time a supersu was granted.

## And now?

your OS is ready, you can follow [GPIO'4'Cars](github.com/zyssai/GPIO-4-Cars/blob/main/v1.5c/readme.md) tutorial.
