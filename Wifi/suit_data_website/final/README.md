# Space Suit Diagnostics

## Features

- [X] 🤩 Page Transitions, component-based (with no-js support)
- [X] 👮‍♂️ 'IntersectionObserver', component-based (with polyfill)
- [X] 🌿 React Context for global UI state, with SSR
- [X] 💅 ['styled-components'](https://www.styled-components.com/) v4
- [X] 💯 Optimized with [Google Lighthouse](https://developers.google.com/web/tools/lighthouse/) (including test)
- [X] 🔥 Code Splitting of CSS and JS (component based)
- [X] 🔪 Inline SVG support
- [X] ⚙️ One config file for site-wide settings
- [X] 💙 Most social + meta tags in one component
- [X] 🖼 All favicons generated, only one icon file needed
- [X] 🌐 Offline support
- [X] 📄 Manifest support
- [X] 🗺 Sitemap support
- [X] 📱 Generated media queries for easy use
- [X] 😎 [Prettier](https://prettier.io/) for code style
- [X] 👷‍♂️ [CircleCI](https://circleci.com/) support
- [X] 🐙 Schema JSONLD
- [X] 🔎 ['size-plugin'](https://github.com/GoogleChromeLabs/size-plugin) to keep an eye on your bundle sizes
- [X] 👨‍🏫 ESLint (based on ['eslint-plugin-react'](./.eslintrc))

Do you have suggestions or feedback? [Open an issue](https://github.com/fabe/gatsby-universal/issues/new)!

## Usage

[![Deploy to Netlify](https://www.netlify.com/img/deploy/button.svg)](https://app.netlify.com/start/deploy?repository=https://github.com/jschmidtnj/spacesuits)

'''bash
# Installation with 'gatsby-cli'
gatsby new my-site https://github.com/jschmidtnj/spacesuits

# Installation with 'git clone'
git clone my-site git@github.com:jschmidtnj/spacesuits.git
cd my-site
yarn install

# To develop
yarn develop

# To build
yarn build

# To test SSR (for Lighthouse etc.)
yarn ssr

# To format JS (precommit)
yarn format

# To generate favicons (included in 'build')
yarn build:favicons
'''

## Configuration

Find the site-wide configuration in 'site-config.js'.

'''js
module.exports = {
  siteTitle: 'Space Suit Diagnostics',
  siteTitleShort: 'SpaceData',
  siteDescription: 'Space Suit Diagnostic Data.',
  siteUrl: 'https://spacesuit.site',
  themeColor: '#000',
  backgroundColor: '#fff',
  pathPrefix: null,
  logo: path.resolve(__dirname, 'src/images/icon.png'),
  social: {
    twitter: 'gatsbyjs',
    fbAppId: '966242223397117',
  },
};
'''

> 🚨 Don't forget to update your 'robots.txt' inside 'static/'!

## Folder structure
'''bash
├── gatsby-browser.js # Specify how Gatsby renders pages in the browser
├── gatsby-config.js # Gatsby config, mostly taken from 'site-config.js'
├── gatsby-node.js # Modify webpack config
├── gatsby-ssr.js # Specify how Gatsby builds pages
├── site-config.js # Global settings for the whole site, used by multiple scripts
├── content # Content & data, in both json and markdown
├── src
│   ├── components
│   │   ├── head # All meta tags etc.
│   │   ├── io # Intersection Observer component, uses render props
│   │   ├── layout # Layout component
│   │   │   ├── layout.css.js # .css.js for component's 'styled-components'
│   │   │   └── layout.js
│   │   └── transition # Page Transition component, used by Gatsby APIs
│   ├── constants # Site-wide constants (breakpoints, colors, etc.)
│   ├── containers # Container components if store is needed
│   ├── helpers
│   │   ├── schemaGenerator.js # Generates JSON-LD schema.org snippets
│   │   └── mediaTemplates.js # Creates media queries for styled-components
│   ├── images # Images needed by the site/theme (not content)
│   ├── pages
│   ├── store # Store and provider of a React.createContext instance
│   └── global.css.js # Global CSS
└── scripts
    ├── lighthouse.test.js # Tests the site specified inside 'site-config.js' with Google Lighthouse (WIP)
    └── favicons.js # Generates favicons and manifest using one png only.
'''
