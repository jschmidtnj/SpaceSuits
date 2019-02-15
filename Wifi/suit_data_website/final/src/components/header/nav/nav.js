import React from 'react';
import { navigate } from 'gatsby';
import { Container } from './nav.css';
const config = require('../../../../site-config');

// unfortunately due to router problems with Cordova (https://github.com/reach/router/issues/25 and https://stackoverflow.com/a/34419824),
// redirecting to the about page in the app doesn't work. It needs to be on the same page.
// https://gist.github.com/twilson63/f32d798bc8aaed6a020011f4a1543e20

let aboutRef = (
  <a onClick={() => navigate('/about')} role="button" tabIndex="0" href="#">
    About
  </a>
);

if (config.cordova) {
  aboutRef = <a href="https://spacesuit.site/about">About</a>;
}

const Nav = () => (
  <Container>
    <ul>
      <li>{aboutRef}</li>
      <li>
        <a href="https://github.com/fabe/gatsby-universal">GitHub</a>
      </li>
    </ul>
  </Container>
);

export default Nav;
