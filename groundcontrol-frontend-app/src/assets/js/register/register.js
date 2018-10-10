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
require("firebase/functions");

firebase.initializeApp(config.firebase);

var provider = new firebase.auth.GoogleAuthProvider();
firebase.auth().useDeviceLanguage();

function handleError(error) {
    // Handle Errors here.
    var errorCode = error.code;
    var errorMessage = error.message;
    //console.log(errorCode, errorMessage);
    var customMessage = "";
    if (errorMessage !== "" && errorMessage !== undefined && errorMessage !== null) {
        customMessage = errorMessage;
    }
    if (errorCode == "auth/email-already-in-use") {
        customMessage = "Email already in use";
    } else if (errorCode == "auth/bad-email-user") {
        customMessage = "Email address not permitted";
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
    $('#alertregistrationfailure').fadeIn();
    setTimeout(function () {
        $('#alertregistrationfailure').fadeOut();
        /*
        setTimeout(function () {
            //console.log("redirecting to registration")
            window.location.href = 'register.html';
        }, config.other.redirecttimeout);
        */
    }, config.other.alerttimeout);
}

function signInGoogle() {
    firebase.auth().signInWithPopup(provider).then(function (result) {
        // This gives you a Google Access Token. You can use it to access the Google API.
        // var token = result.credential.accessToken;
        // The signed-in user info.
        var user = result.user;
        var email = user.email;
        //console.log("email: " + email);
        if (email == "") { //change to regex email verification
            user.delete().then(function () {
                firebase.auth().signOut().then(function () {
                    //console.log('Signed Out');
                    handleError({
                        code: "auth/bad-user",
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
                window.location.href = 'dashboard.html';
            }
        }
    }).catch(function (error) {
        // Handle Errors here.
        //var errorCode = error.code;
        //var errorMessage = error.message;
        // The email of the user's account used.
        //var email = error.email;
        //console.log("error with google sign-in");
        handleError(error);
    });
}

$(document).ready(function () {

    $('#toslink').attr('href', config.other.tosUrl);
    $('#privacypolicylink').attr('href', config.other.privacyPolicyUrl);

    $.validator.addMethod(
        "regex1",
        function (value, element, regexp) {
            var re = new RegExp(regexp, 'i');
            return this.optional(element) || re.test(value);
        },
        ""
    );

    $.validator.addMethod(
        "regex2",
        function (value, element, regexp) {
            var re = new RegExp(regexp, 'i');
            return this.optional(element) || re.test(value);
        },
        ""
    );

    $.validator.addMethod(
        "regex3",
        function (value, element, regexp) {
            var re = new RegExp(regexp, 'i');
            return this.optional(element) || re.test(value);
        },
        ""
    );

    //create terms of service and privacy policy links
    $('#toslink').attr('href', config.other.tosUrl);
    $('#privacypolicylink').attr('href', config.other.privacyPolicyUrl);
    $('#helplink').attr('href', config.other.helpPageUrl);

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

    var usertype = "";

    function addUser(formData) {
        var name = formData[0].value.toString();
        var fistandlast = formData[0].value.toString().split(" ");
        var email = formData[1].value.toString();
        firebase.auth().createUserWithEmailAndPassword(email, formData[2].value).then(function () {
            //save data to database
            var user = firebase.auth().currentUser;
            user.updateProfile({
                displayName: name,
                email: email
            }).then(function () {
                // Update successful.
                var userId = user.uid;
                firebase.database().ref('users/' + userId).set({
                    name: name,
                    firstname: fistandlast[0],
                    lastname: fistandlast.slice(1).join(' '),
                    email: email,
                    signintype: "email",
                    usertype: usertype
                }).then(function () {
                    user.sendEmailVerification().then(function () {
                        // Email sent.
                        //console.log("successfully registered!")
                        firebase.auth().signOut().then(function () {
                            //console.log('Signed Out');
                            $('#alertsuccessregistered').fadeIn();
                            setTimeout(function () {
                                $('#alertsuccessregistered').fadeOut();
                                setTimeout(function () {
                                    //console.log("redirecting to login")
                                    window.location.href = 'login.html';
                                }, config.other.redirecttimeout);
                            }, config.other.alerttimeout);
                        }).catch(function (error) {
                            //console.error('Sign Out Error', error);
                            handleError(error);
                        });
                    }).catch(function (error) {
                        // An error happened.
                        handleError(error);
                    });
                }).catch(function (error) {
                    user.delete().then(function () {
                        firebase.auth().signOut().then(function () {
                            //console.log('Signed Out');
                            handleError({
                                code: "auth/bad-email-user",
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
                });
            }).catch(function (error) {
                // An error happened.
                handleError(error);
            });
        }).catch(function (error) {
            handleError(error);
        });
    }

    function checkKey(formData, secretKey) {
        //console.log("sending submission email notification");
        var checkSecretKey = firebase.functions().httpsCallable('checksecretkey');
        checkSecretKey({
            secretKey: secretKey
        }).then(function (result) {
            // Read result of the Cloud Function.
            //console.log(result);
            var statusMessage = result.data.status;
            //console.log(statusMessage);
            if (statusMessage == "valid") {
                addUser(formData, usertype);
            } else {
                //console.log("invalid key");
                handleError({
                    code: "auth/invalid-key",
                    message: "Key not valid"
                });
            }
        }).catch(function (error) {
            // Getting the Error details.
            //console.log(error);
            handleError(error);
        });
    }

    function validateForm() {
        $("#registerForm").validate({
            rules: {
                fullname: {
                    required: true,
                    regex1: config.regex.fullname
                },
                secretKey: {
                    required: true
                },
                password: {
                    required: true,
                    minlength: 6,
                    maxlength: 15,
                    regex1: config.regex.passwordcontainsletter,
                    regex2: config.regex.passwordcontainsnumber,
                    regex3: config.regex.passwordcontainsspecialcharacter
                },
                confirm_password: {
                    required: true,
                    equalTo: "#password"
                },
                email: {
                    required: true,
                    regex1: config.regex.validemail
                },
                agree: "required"
            },
            messages: {
                fullname: "Please enter your full name",
                secretkey: {
                    required: "Please enter a secret key"
                },
                password: {
                    required: "Please provide a password",
                    minlength: "Your password must be at least 6 characters long",
                    maxlength: "Your password cannot exceed 15 characters",
                    regex1: "Your password must contain at least one alpha-numerical character",
                    regex2: "Your password must contain at least one digit (0-9)",
                    regex3: "Your password must contain at least one special character (!@#$%^&*)"
                },
                confirm_password: {
                    required: "Please provide a password",
                    equalTo: "Please enter the same password as above"
                },
                email: "Please enter a valid email address",
                agree: "Please accept our privacy polity"
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
    }

    $("#submitData").on('click touchstart', function () {
        if ($("#registerForm").valid()) {
            var formData = $("#registerForm").serializeArray();
            //console.log(formData);
            if (usertype == "doctor" || usertype == "insurance") {
                var secretKeyInput = formData[4].value.toString();
                checkKey(formData, secretKeyInput);
            } else {
                addUser(formData);
            }
        } else {
            console.log("form invalid");
        }
    });

    var expandedForm = 0; //collapsed = 0

    $("#registerPatient").on('click touchstart', function () {
        usertype = "patient";
        if (expandedForm == 1) { //if it is mode 1 collapse
            $("#expandForm").addClass("collapse");
            expandedForm = 0;
        } else {
            $("#expandForm").removeClass("collapse");
            expandedForm = 1;
        }
        $("#expandSecretKey").addClass("collapse");
        expandedSecretKey = false;
        validateForm();
    });

    $("#registerDoctor").on('click touchstart', function () {
        usertype = "doctor";
        if (expandedForm == 2) { //if it is mode 2 collapse
            $("#expandForm").addClass("collapse");
            expandedForm = 0;
        } else {
            $("#expandForm").removeClass("collapse");
            expandedForm = 2;
        }
        $("#expandSecretKey").removeClass("collapse");
        validateForm();
    });

    $("#registerInsurance").on('click touchstart', function () {
        usertype = "insurance";
        if (expandedForm == 3) { //if it is mode 3 collapse
            $("#expandForm").addClass("collapse");
            expandedForm = 0;
        } else {
            $("#expandForm").removeClass("collapse");
            expandedForm = 3;
        }
        $("#expandSecretKey").removeClass("collapse");
        validateForm();
    });

});