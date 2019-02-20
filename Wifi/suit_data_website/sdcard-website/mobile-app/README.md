# Cordova App

tested on Ubuntu 18. first run `npm i`. Then run `sudo apt-get install openjdk-8-jre`. Then you need to install the android sdk using [this](https://stackoverflow.com/questions/34556884/how-to-install-android-sdk-on-ubuntu). DO NOT install with apt - it won't work :(.

Follow [these instructions](https://codeburst.io/publish-a-cordova-generated-android-app-to-the-google-play-store-c7ae51cccdd5) for getting the app signed and stuff.

## config

If emulator is needed in virtual machine go to vm settings -> processor -> virtualization and enable. Then run `SVGA_VGPU10=0` in machine.
Run `sudo chown <username> -R /dev/kvm`.

## build

Build application with `npm run build-cordova`. Emulate with `cordova emulate android`.

Build apk like this:

* `cordova build --release`
* `jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore android.keystore '/home/joshua/Desktop/SpaceSuits/Wifi/suit_data_website/final/mobile-app/platforms/android/app/build/outputs/apk/release/app-release-unsigned.apk'  android-app-key`
* `zipalign -v 4 app-release-unsigned.apk app-release.apk`

## troubleshooting

unfortunately due to router problems with Cordova (https://github.com/reach/router/issues/25 and https://stackoverflow.com/a/34419824),
redirecting to the about page in the app doesn't work. It needs to be on the same page.
https://gist.github.com/twilson63/f32d798bc8aaed6a020011f4a1543e20