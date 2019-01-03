var connect = require('connect');
var serveStatic = require('serve-static');
connect().use(serveStatic("dist", {'index': ['index.html', 'index.htm']})).listen(8000, function(){
    console.log('Server running on 8000...');
});