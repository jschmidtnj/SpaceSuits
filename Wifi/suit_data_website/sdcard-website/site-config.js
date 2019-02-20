const path = require('path');

module.exports = {
  siteTitle: 'SpaceSuit Diagnostics',
  siteTitleShort: 'Suit Stats',
  siteDescription: 'Suit Data and Diagnostics Info.',
  siteUrl: 'https://spacesuit.site',
  themeColor: '#000',
  backgroundColor: '#fff',
  pathPrefix: null,
  logo: path.resolve(__dirname, 'src/images/icon.png'),
  social: {
    twitter: 'gatsbyjs',
    fbAppId: '966242223397117',
  },
  dataInterval: 5000, //ms
  getSuitDataUrl: 'http://192.168.1.1/getsuitdata',
  getTaskDataUrl: 'http://192.168.1.1/gettaskdata',
  cordova: false, // true = build for cordova, false = otherwise - cordova is not working correctly at the moment
  // react native and vue nativescript also didn't work for creating app because of problems with http requests without internet
  // connection, so I am giving up on the native thing at the moment and instead using an electron app.
};
