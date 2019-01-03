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
};
