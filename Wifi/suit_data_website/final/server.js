var connect = require('connect');
var serveStatic = require('serve-static');
connect().use(serveStatic("public", {'index': ['index.html', 'index.htm']})).listen(8080, function(){
    console.log('Server running on 8080...');
});