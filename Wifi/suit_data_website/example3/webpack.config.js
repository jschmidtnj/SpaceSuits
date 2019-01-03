require('webpack');
const path = require('path');
require('uglifyjs-webpack-plugin');
const WebpackSweetEntry = require('webpack-sweet-entry');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const WebpackPwaManifest = require('webpack-pwa-manifest');

const sourcePath = path.join(__dirname, 'src');
const buildPath = path.join(__dirname, 'dist');

const appicon = path.join(__dirname, 'src/assets/icons/pwa_icons/app_icon_512.png');
const apptitle = 'Spacesuit Diagnostics'

module.exports = [
  {
    plugins: [
      new HtmlWebpackPlugin({
        filename: 'index.html',
        template: 'src/html/index.html',
      }),
      new WebpackPwaManifest({
        name: apptitle,
        short_name: 'Suit Data',
        description: 'Space Suit Diagnostics Panel',
        background_color: '#009999',
        crossorigin: 'use-credentials', //can be null, use-credentials or anonymous
        icons: [
          {
            src: appicon,
            sizes: [96, 128, 192, 256, 384, 512]
          },
          {
            src: appicon,
            sizes: [120, 152, 167, 180, 1024],
            destination: path.join('icons', 'ios'),
            ios: true
          },
          {
            src: appicon,
            size: 1024,
            destination: path.join('icons', 'ios'),
            ios: 'startup'
          },
          {
            src: appicon,
            sizes: [36, 48, 72, 96, 144, 192, 512],
            destination: path.join('icons', 'android')
          }
        ],
        options: {
          filename: "manifest.json",
          name: "App",
          orientation: "portrait",
          display: "standalone",
          start_url: ".",
          crossorigin: null,
          inject: true,
          fingerprints: true,
          ios: {
            'apple-mobile-web-app-title': apptitle,
            'apple-mobile-web-app-status-bar-style': 'white'
          },
          publicPath: null,
          includeDirectory: true
        }
      })
    ]
  },
  {
    node: {
      fs: 'empty'
    },
    entry: WebpackSweetEntry(path.resolve(sourcePath, 'assets/js/**/*.js*'), 'js', 'js'),
    output: {
      path: path.resolve(buildPath, 'assets/js'),
      filename: '[name].js'
    },
    module: {
      rules: [{
          test: /\.css$/,
          use: [
            'style-loader',
            'css-loader'
          ]
        },
        {
          test: /\.(png|jpg|gif)$/,
          use: [
            'file-loader'
          ]
        },
        {
          test: /\.(ttf|eot|svg)(\?v=[0-9]\.[0-9]\.[0-9])?$/,
          use: [
            'file-loader'
          ]
        },
        {
          test: /\.woff(2)?(\?v=[0-9]\.[0-9]\.[0-9])?$/,
          use: [
            'url-loader?limit=10000&mimetype=application/font-woff'
          ]
        },
        {
          test: /\.(csv|tsv)$/,
          use: [
            'csv-loader'
          ]
        },
        {
          test: /\.xml$/,
          use: [
            'xml-loader'
          ]
        }
      ]
    }
  },
  {
    entry: WebpackSweetEntry(path.resolve(sourcePath, 'assets/scss/**/*.scss'), 'scss', 'scss'),
    output: {
      path: path.resolve(buildPath, 'assets/scss'),
      filename: '[name].js' //scss file created acts like a js file
    },
    module: {
      rules: [{
          test: /\.(scss)$/,
          use: [{
            loader: 'style-loader', // inject CSS to page
          }, {
            loader: 'css-loader', // translates CSS into CommonJS modules
          }, {
            loader: 'postcss-loader', // Run post css actions
            options: {
              plugins: function () { // post css plugins, can be exported to postcss.config.js
                return [
                  require('precss'),
                  require('autoprefixer')
                ];
              }
            }
          }, {
            loader: 'sass-loader' // compiles Sass to CSS
          }]
        },
        {
          test: /.(ttf|otf|eot|svg|woff(2)?)(\?[a-z0-9]+)?$/,
          use: [{
            loader: 'file-loader',
            options: {
              name: '[name].[ext]',
              outputPath: '../fonts/', // where the fonts will go,
              publicPath: '../'       // override the default path
            }
          }]
        },
      ]
    }
  }
];