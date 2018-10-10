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

$(document).ready(function () {

    $('#toslink').attr('href', config.other.tosUrl);
    $('#privacypolicylink').attr('href', config.other.privacyPolicyUrl);
    $('#helplink').attr('href', config.other.helpPageUrl);

    $.validator.addMethod(
        "regex1",
        function (value, element, regexp) {
            var re = new RegExp(regexp, 'i');
            return this.optional(element) || re.test(value);
        },
        ""
    );

    function createTaskFormValidation() {
        $("#taskForm").validate({
            rules: {
                task: {
                    required: true
                }
            },
            messages: {
                task: "Please enter a task"
            },
            errorElement: "div",
            errorPlacement: function (error, element) {
                // Add the `invalid-feedback` class to the div element
                error.addClass("invalid-feedback");
    
                if (element.prop("type") === "checkbox") {
                    error.insertAfter(element.parent("label"));
                } else {
                    error.insertAfter(element);
                }
            },
            highlight: function (element) {
                $(element).addClass("is-invalid").removeClass("is-valid");
            },
            unhighlight: function (element) {
                $(element).addClass("is-valid").removeClass("is-invalid");
            }
        });

        function submitTask() {
            if ($("#taskForm").valid()) {
                var formData = $("#registerForm").serializeArray();
                //console.log(formData);
                var task = formData[0].value.toString();
                firebase.database().ref('tasks/').push({
                    data: task
                }).then(function() {
                    console.log("task added");
                }).catch(function(err){
                    handleError(err);
                });
            } else {
                console.log("form invalid");
            }
        }

        $("#submitTask").on('click touchstart', function () {
            submitTask();
        });

        $("#task").keypress(function (event) {
            if (event.which == '13') {
                event.preventDefault();
                submitTask();
            }
        });
    }

    function getSuitData() {
        console.log("get the suit data");
        var suitdataref = firebase.database().ref("suitdata");
        suitdataref.on("value", function(suitdata) {
            var suitdataval = suitdata.val();
            console.log(suitdataval);
            var primaryo2 = suitdataval.primaryo2;
            $("#primaryo2").prop('disabled', false);
            $("#primaryo2").val(primaryo2);
            $("#primaryo2").prop('disabled', true);
            var secondaryo2 = suitdataval.secondaryo2;
            $("secondaryo2").prop('disabled', false);
            $("#secondaryo2").val(secondaryo2);
            $("#secondaryo2").prop('disabled', true);
            var battery = suitdataval.battery;
            $("#battery").prop('disabled', false);
            $("#battery").val(battery);
            $("#battery").prop('disabled', true);
            var heartrate = suitdataval.heartrate;
            $("#heartrate").prop('disabled', false);
            $("#heartrate").val(heartrate);
            $("#heartrate").prop('disabled', true);
            var moisture = suitdataval.moisture;
            $("#moisture").prop('disabled', true);
            $("#moisture").val(moisture);
            $("#moisture").prop('disabled', true);

            var accelx = suitdataval.accelx;
            $("#accelx").prop('disabled', false);
            $("#accelx").val(accelx);
            $("#accelx").prop('disabled', true);
            var accely = suitdataval.accely;
            $("#accely").prop('disabled', false);
            $("#accely").val(accely);
            $("#accely").prop('disabled', true);
            var accelz = suitdataval.accelz;
            $("#accelz").prop('disabled', false);
            $("#accelz").val(accelz);
            $("#accelz").prop('disabled', true);
            var roll = suitdataval.roll;
            $("#roll").prop('disabled', false);
            $("#roll").val(roll);
            $("#roll").prop('disabled', true);
            var pitch = suitdataval.pitch;
            $("#pitch").prop('disabled', false);
            $("#pitch").val(pitch);
            $("#pitch").prop('disabled', true);
            var yaw = suitdataval.yaw;
            $("#yaw").prop('disabled', false);
            $("#yaw").val(yaw);
            $("#yaw").prop('disabled', true);
        });
    }

    var signed_in_initially = false;

    firebase.auth().onAuthStateChanged(function (user) {
        if (user) {
            window.user = user;
            signed_in_initially = true;
            firebase.database().ref('users/' + user.uid).once('value').then(function (userdata) {
                var userdataval = userdata.val();
                var usertype = userdataval.usertype;
                if (usertype == "groundcontrol") {
                    console.log("ground control signed in");
                }
            }).catch(function(err) {
                handleError(err);
            });
            $("#bodycollapse").removeClass("collapse");
            $("#taskmanagementcollapse").removeClass("collapse");
            createTaskFormValidation();
            $("#suitdatacollapse").removeClass("collapse");
            getSuitData();
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

});