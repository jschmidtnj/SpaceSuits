var jQuery = require("jquery");
require('jquery-validation');
require("popper.js");
require("bootstrap");
window.$ = window.jQuery = jQuery;
var PDFObject = require('pdfobject');

var config = require('../../../config/config.json');

if (PDFObject.supportsPDFs) {
    console.log("Yay, this browser supports inline PDFs.");
    var options = {
        pdfOpenParams: {
            pagemode: "thumbs",
            navpanes: 0,
            toolbar: 0,
            statusbar: 0,
            view: "FitV"
        }
    };

    PDFObject.embed("assets/reports/proposal.pdf", "#proposal", options);
    //var el = document.querySelector("#results");
    //el.setAttribute("class", (PDFObject) ? "success" : "fail");
    //el.innerHTML = (PDFObject) ? "PDFObject successfully added an &lt;embed> element to the page!" : "Uh-oh, the embed didn't work.";
} else {
    console.log("Boo, inline PDFs are not supported by this browser");
}