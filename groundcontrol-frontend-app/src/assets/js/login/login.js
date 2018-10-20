var jQuery = require('jquery');
require('jquery-validation');
require("popper.js");
require("bootstrap");
var $script = require('scriptjs');
window.$ = window.jQuery = jQuery;

$script.path('assets/js/');
$script('global/loginCSSHelper.js');

var config = require('../../../config/config.json');

var firebase = require("firebase/app");
require("firebase/auth");
require("firebase/database");

firebase.initializeApp(config.firebase);

var provider = new firebase.auth.GoogleAuthProvider();
firebase.auth().useDeviceLanguage();

function handleError(error) {
    // Handle Errors here.
    var errorCode = error.code;
    var errorMessage = error.message;
    //console.log(errorCode, errorMessage);
    var customMessage = "";
    if (errorCode == "auth/wrong-password") {
        customMessage = "Wrong Password";
    } else if (errorCode == "auth/user-not-found") {
        customMessage = "Wrong Email";
    } else if (errorCode == "auth/email-not-verified" || errorCode == "auth/user-not-found" || errorCode == "auth/bad-user") {
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
    $('#alertloginfailure').fadeIn();
    setTimeout(function () {
        $('#alertloginfailure').fadeOut();
    }, config.other.alerttimeout);
}

function signInGoogle() {
    firebase.auth().signInWithPopup(provider).then(function (result) {
        // This gives you a Google Access Token. You can use it to access the Google API.
        // var token = result.credential.accessToken;
        // The signed-in user info.
        var user = result.user;
        var email = user.email;
        if (email == "") { //change to regex email verification
            user.delete().then(function () {
                firebase.auth().signOut().then(function () {
                    //console.log('Signed Out');
                    handleError({
                        errorCode: "auth/bad-user",
                        message: "Email not permitted"
                    });
                }, function (error) {
                    //console.error('Sign Out Error', error);
                    handleError(error);
                });
            }, function (error) {
                //console.error('Sign Out Error', error);
                handleError(error);
            });
        } else {
            var metadata = firebase.auth().currentUser.metadata;
            //console.log("metadata: " + metadata);
            //console.log(metadata.creationTime, metadata.lastSignInTime)
            if (metadata.creationTime == metadata.lastSignInTime) {
                //first sign-in attempt
                //console.log("first sign-in");
                //add information to users database
                var userId = user.uid;
                var name = user.displayName;
                var notificationsettingsData = {
                    emailon: true
                };
                firebase.database().ref('users/' + userId).set({
                    name: name,
                    email: email,
                    signintype: "google",
                    notificationsettings: notificationsettingsData
                }).then(function () {
                    //console.log("success - user signed in");
                    window.location.href = window.urlRedirect;
                }).catch(function (error) {
                    handleError(error);
                });
            } else {
                //console.log("signing in! " + email);
                window.location.href = window.urlRedirect;
            }
        }
    }).catch(function (error) {
        // Handle Errors here.
        //var errorCode = error.code;
        //var errorMessage = error.message;
        // The email of the user's account used.
        //var email = error.email;
        handleError(error);
    });
}

function checkIfLoggedin() {
    setTimeout(function () {
        var user = firebase.auth().currentUser;
        if (user != null) {
            //console.log("already logged in");
            //console.log("redirecting to dashboard");
            window.location.href = window.urlRedirect;
        } else {
            //console.log("need to login");
        }
    }, 500);
}

$(document).ready(function () {

    $('#toslink').attr('href', config.other.tosUrl);
    $('#privacypolicylink').attr('href', config.other.privacyPolicyUrl);
    $('#helplink').attr('href', config.other.helpPageUrl);

    window.urlRedirect = "dashboard.html";
    checkIfLoggedin();

    $("#loginSubmitGoogle").on('click touchstart', function () {
        //console.log("signing into google");
        firebase.auth().signOut().then(function () {
            // Sign-out successful.
            //console.log("sign out success");
            signInGoogle();
        }).catch(function (error) {
            // An error happened.
            //console.log("sign out error occurred: " + error.message);
            signInGoogle();
        });

    });

    function loginsubmit() {
        if ($("#loginForm").valid()) {
            //console.log("valid input");
            var formData = $("#loginForm").serializeArray();
            var email = formData[0].value.toString();
            var password = formData[1].value.toString();
            firebase.auth().signInWithEmailAndPassword(email, password).then(function () {
                var user = firebase.auth().currentUser;
                if (user) {
                    var emailVerified = user.emailVerified;
                    if (emailVerified) {
                        $('#alertloginsuccess').fadeIn();
                        setTimeout(function () {
                            $('#alertloginsuccess').fadeOut();
                            setTimeout(function () {
                                //console.log("redirecting to dashboard");
                                window.location.href = window.urlRedirect;
                            }, config.other.redirecttimeout);
                        }, config.other.alerttimeout);
                    } else {
                        firebase.auth().signOut().then(function () {
                            // Sign-out successful.
                            handleError({
                                code: "auth/email-not-verified",
                                message: "Email not verified"
                            });
                        }).catch(function (error) {
                            // An error happened.
                            handleError(error);
                        });
                    }
                } else {
                    handleError({
                        code: "auth/user-not-found",
                        message: "User not found"
                    });
                }
            }).catch(function (error) {
                handleError(error);
            });
        } else {
            //console.log("invalid input");
        }
    }

    $("#password").keypress(function (event) {
        if (event.which == '13') {
            event.preventDefault();
            loginsubmit();
        }
    });

    var loginSubmitted = false;
    $("#loginSubmitEmail").on('click touchstart', function (e) {
        e.stopImmediatePropagation();
        if(e.type == "touchstart") {
            loginSubmitted = true;
            loginsubmit();
        }
        else if(e.type == "click" && !loginSubmitted) {
            loginsubmit();
        }
        else {
            loginSubmitted = false;
        }
        return false;
    });

    $.validator.addMethod(
        "regex",
        function (value, element, regexp) {
            var re = new RegExp(regexp, 'i');
            return this.optional(element) || re.test(value);
        },
        ""
    );

    $("#loginForm").validate({
        rules: {
            email: {
                required: true,
                regex: config.regex.validemail,
                maxlength: 50
            },
            password: {
                required: true,
                minlength: 6,
                maxlength: 50
            }
        },
        messages: {
            email: "Please enter a valid email address",
            password: "Please provide a valid password"
        },
        errorElement: "div",
        errorPlacement: function (error, element) {
            // Add the `invalid-feedback` class to the div element
            error.addClass("invalid-feedback");
            error.insertAfter(element);
        },
        highlight: function (element, errorClass, validClass) {
            $(element).addClass("is-invalid").removeClass("is-valid");
        },
        unhighlight: function (element, errorClass, validClass) {
            $(element).addClass("is-valid").removeClass("is-invalid");
        }
    });
});
