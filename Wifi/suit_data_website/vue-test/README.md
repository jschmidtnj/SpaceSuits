# Livestream VTC connection

## Build Setup

``` bash
# install dependencies
npm install

# serve with hot reload at localhost:8080
npm run dev

# build for production with minification
npm run build

# build for production and view the bundle analyzer report
npm run build --report

# run unit tests
npm run unit

# run e2e tests
npm run e2e

# run all tests
npm test

# run format
npm run format

# run server
npm run server
```

## Methodology

Customers requested the development of VTC (Video TeleConferencing) software to be used in ANTYX and the demonstration. Various technologies were examined including different transport protocols (udp, rtsp, rtp, websockets), encodings and file types (h.264, mp4, m3u8, mpegts, aac), splitting the video, and real-time transcoding (ffmpeg). However, all of these different technologies had their drawbacks, most notably latancy. Even the fastest video streaming protocol - UDP - was bottlenecked because the transcoding of the video from the webcam was too slow. The only protocol / technology that worked with little latancy even when sent over a poor radio connection (100 kbps) was WebRTC.

## webrtc

Configuring webrtc is usually a challenge because there are so many pieces involved. Session Traversal Utilities for NAT (STUN) servers and Traversal Using Relays around NAT (TURN) servers are required to resolve public and private ip addresses, even when using these technologies in a local network. Originally, the idea was to use [the apprtc project](https://github.com/webrtc/apprtc) to start development, but the configuration of STUN and TURN servers for a local development environment proved to not be possible, due to problems with local unsigned ssl certificates and chrome, in addition to the way apprtc handles secure websockets. However, another project - [RTCMulticonnection](https://github.com/muaz-khan/RTCMultiConnection) worked locally with little additional configuration because it was designed to integrate STUN, TURN and ICE servers. These servers were all implemented in the library, allowing for a simple express.js server to handle the client-side application, with socket-io acting as the liaison.

## rtcmulticonnection

RTCMulticonnection is relatively straight-forward to use and configure. All that is needed is an html page with some client-side javascript and the correct required scripts and configuration. Documentation can be found [here](https://github.com/muaz-khan/RTCMultiConnection/wiki/Bandwidth-Management), and an example is shown on the bottom of this page. Please note that for the example node_modules are needed, and a server is required to serve the page. The example used is source controlled but is not currently saved in a remote repository for security reasons.

## gatsby.js

Gatsby.js is a react-based static site generator. The idea was to use this to create the necessary website in a modular way, as a wrapper over the rtcmulticonnection code. However, the React component did not work correctly with the media element, as it requires direct access to the DOM structure. From what I can see there is not much that can be done to make it work correctly. That is why I decided to move on to webpack and another framework that may work better.

## vue.js framework

Vue.js was used as a framework / wrapper for this application, to make it easier to develop in the future. The code is based on [this template](https://github.com/vuejs-templates/webpack), and runs in the main app script. To further style the website, create more Vue components and style the livestream manually. For a detailed explanation on how vue.js and webpack works, check out the [guide](http://vuejs-templates.github.io/webpack/) and [docs for vue-loader](http://vuejs.github.io/vue-loader).

## config

install with `npm i`, start with `node server.js --port=80 --ssl` or more simply `npm run start`. Then navigate to `https://localhost:80` on host computer and `https://<ip-address-of-host>:80/`. Make sure firewall rules in Windows or the given os allow for all ports to be opened, or at least the ports that are needed (443 and 80), both inbound and outbound, on both computers. Also enable self-signed flags in chrome by going to `chrome://flags/#allow-insecure-localhost`. The tested working browser is Chrome.

## certificates

try looking at [this](https://superuser.com/questions/1303396/how-to-fix-firefox-59-no-longer-accepting-my-self-signed-ssl-certificate-on-dev) for firefox. Also use this command: `openssl req -x509 -nodes -days 730 -newkey rsa:2048  -keyout privatekey.pem -out certificate.pem -config req.cnf -sha256` in the `fake-keys` directory to create the keys. Go to chrome settings -> advanced -> manage certificates. Import certificate in trusted root certification authorities, using the fake-keys files.