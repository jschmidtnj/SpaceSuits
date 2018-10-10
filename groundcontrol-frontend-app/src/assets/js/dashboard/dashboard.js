var jQuery = require("jquery");
require('jquery-validation');
require("popper.js");
require("bootstrap");
window.$ = window.jQuery = jQuery;

require("bootstrap-select");
require("bootstrap-select/dist/css/bootstrap-select.css");

var firebase = require("firebase/app");
require("firebase/auth");
require("firebase/database");
var config = require('../../../config/config.json');
firebase.initializeApp(config.firebase);

function handleError(error) {
    // Handle Errors here.
    var errorCode = error.code;
    var errorMessage = error.message;
    //console.log(errorCode, errorMessage);
    var customMessage = "";
    if (errorCode == "auth/notsignedin") {
        customMessage = errorMessage;
    }
    if (error.code !== "" && error.message !== "") {
        if (customMessage !== "") {
            $('#error-info').text(customMessage);
        } else {
            $('#error-info').text("Error: " + errorMessage + " Code: " + errorCode);
        }
    } else {
        $('#error-info').text("No Error code found.");
    }
    $('#alertsignoutfailure').fadeIn();
    setTimeout(function () {
        $('#alertsignoutfailure').fadeOut();
    }, config.other.alerttimeout);
}

var createdDownloadButton = false;

$(document).ready(function () {

    $('#toslink').attr('href', config.other.tosUrl);
    $('#privacypolicylink').attr('href', config.other.privacyPolicyUrl);
    $('#helplink').attr('href', config.other.helpPageUrl);

    var signed_in_initially = false;

    firebase.auth().onAuthStateChanged(function (user) {
        if (user) {
            window.user = user
            signed_in_initially = true;
            var ref = firebase.database().ref('users/' + user.uid);
            ref.once('value').then(function (snapshot) {
                console.log("snapshot", snapshot.val().usertype)
                var usertype = snapshot.val().usertype;
                if (usertype == "patient") {
                    console.log("Patient signed in");
                    // User is signed in.
                    //console.log("signed in");
                    $("#bodycollapse").removeClass("collapse");
                    $("#patientDash").removeClass("collapse");
                    firebase.database().ref("requests/").once('value').then(function (snapshot) {
                        snapshot.forEach(function (childSnapshot) {
                            var patientId = childSnapshot.val().patientID;
                            if (patientId == window.user.uid) {
                                console.log(childSnapshot.val())
                                var doctor = childSnapshot.val().doctor;
                                $("#notificationsBody").append(
                                    "<tr><td>" + doctor + "</td><td><button id=\"" + patientId
                                     + "\" class='accept btn btn-primary'>Accept</button></td>" + 
                                     "<td><button id=\"" + patientId + "\" class='reject btn btn-primary'>Reject</button></td></tr>"
                                );
                            }
                        })
                    })
                } else if (usertype == "doctor") {
                    window.email = user.email;
                    var testemail = new RegExp(config.regex.adminemailregex, 'g');
                    $("#bodycollapse").removeClass("collapse");
                    $("#doctorDash").removeClass("collapse");
                }
            });
        } else {
            // No user is signed in. redirect to login page:
            if (signed_in_initially) {
                $('#alertsignoutsuccess').fadeIn();
                setTimeout(function () {
                    $('#alertsignoutsuccess').fadeOut();
                    //console.log("redirecting to login page");
                    setTimeout(function () {
                        window.location.href = 'login.html';
                    }, config.other.redirecttimeout);
                }, config.other.alerttimeout);
            } else {
                //slow redirect
                handleError({
                    code: "auth/notsignedin",
                    message: "Not Signed in. Redirecting."
                });
                //console.log("redirecting to login page");
                setTimeout(function () {
                    window.location.href = 'login.html';
                }, config.other.redirecttimeout);
                //fast redirect
                // window.location.href = 'login.html';
            }
        }
    });

$("#logoutButton").on('click touchstart', function () {
    firebase.auth().signOut().then(function () {
        // Sign-out successful.
    }).catch(function (error) {
        // An error happened.
        handleError(error);
    });
});

//events for approve and reject buttons
$(".approve").on("click touchstart", function (event) {
    event.preventDefault();
    
    console.log("approve")
})

$("#notifications").on("click", "#reject1", function (event) {
    event.preventDefault();
    var parent = $(event.target).parent()
    console.log("reject")
})

//event for doctor request data
$("#submitDataRequest").on("click", function (event) {
    event.preventDefault();
    var patientID = $("#getPatientData").serializeArray()[0].value;
    firebase.database().ref('users/' + patientID).once('value').then(function (snapshot) {
        var patient = snapshot.val();
        console.log("patient", patient);
        console.log(window.user.uid);
        firebase.database().ref('users/' + window.user.uid).once('value').then(function (snapshot1) {
            var doctorName = snapshot1.val().name;
            console.log("patientID", patientID);
            var requests = firebase.database().ref('requests/');
            var sendRequests = requests.push({
                patientID: patientID,
                doctor: doctorName
            });
            sendRequests;
            console.log("request sent");
        })
    })
});
//submit ehr form
$("#submitEhr").on("click", function (event) {
    event.preventDefault();
    var ehrData = $("#ehrForm").serializeArray()
    console.log(ehrData);
    var ehr = firebase.database().ref('ehr/' + ehrData[0].value);
    var sendEhr = ehr.push({
        name: ehrData[1].value,
        age: ehrData[2].value,
        prescription: ehrData[3].value,
        diagnosis: ehrData[4].value,
        notes: ehrData[5].value,
        other: ehrData[6].value
    })
    sendEhr;
    console.log(sendEhr, "ehr sent")
})
});
