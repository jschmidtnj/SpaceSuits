var jQuery = require("jquery");
require('jquery-validation');
require("popper.js");
require("bootstrap");
window.$ = window.jQuery = jQuery;

require('datatables.net-bs4');
require('datatables.net-responsive-bs4');
require('datatables.net-select-bs4');
require('datatables.net-bs4/css/dataTables.bootstrap4.min.css');
require('datatables.net-responsive-bs4/css/responsive.bootstrap4.min.css');
require('datatables.net-select-bs4/css/select.bootstrap4.min.css');

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

var tableInitialized = false;

function createDoctorTable() {
    var ranonce = false;

    if (tableInitialized) {
        $('#doctorlist').DataTable().destroy();
        tableInitialized = false;
    }

    function generateDatatable() {
        if (!(ranonce)) {
            $("#nodoctorswarning").removeClass("collapse");
            $("#doctorlistcollapse").addClass("collapse");
            $("#selectactions").addClass("collapse");
        } else {
            if (!(tableInitialized)) {
                $('#doctorlist').DataTable({
                    responsive: true,
                    select: {
                        style: 'multi'
                    }
                });
                tableInitialized = true;
            }
        }
    }
    firebase.database().ref('users/' + window.userId + '/doctors/').limitToLast(config.other.doctorviewmax).once('value').then(function (doctors) {
        var numdoctors = doctors.numChildren();
        var countdoctors = 0;
        doctors.forEach(function (doctor) {
            countdoctors++;
            var doctorId = doctor.key;
            //console.log(formId);
            var doctordata = doctor.val();
            var doctorName = doctordata.name;
            $('#doctordata').append("<tr><td>" + doctorName + "</td><td><button value=\"" + doctorId +
                "\" class=\"doctorDelete btn btn-primary btn-block onclick=\"" +
                "void(0)\"\">Delete</button></td></tr>");
            if (!(ranonce)) {
                ranonce = true;
                $("#nodoctorswarning").addClass("collapse");
                $("#doctorlistcollapse").removeClass("collapse");
                $("#selectactions").removeClass("collapse");
            }
            if (numdoctors == countdoctors) {
                generateDatatable();
            }
        });
        setTimeout(function () {
            generateDatatable();
        }, config.other.datatimeout);
    }).catch(function (error) {
        handleError(error);
    });
}

function getInitialValues() {
    firebase.database().ref('users/' + window.userId).once('value').then(function (userData) {
        var userDataVal = userData.val();
        //console.log(userDataVal);
        var name = userDataVal.name;
        $("#fullname").val(name);
        var username = userDataVal.username;
        $("#username").val(username);
        createDoctorTable();
    }).catch(function (err) {
        handleError(err);
    });
}

$(document).ready(function () {

    $('#toslink').attr('href', config.other.tosUrl);
    $('#privacypolicylink').attr('href', config.other.privacyPolicyUrl);
    $('#helplink').attr('href', config.other.helpPageUrl);

    var signed_in_initially = false;
    firebase.auth().onAuthStateChanged(function (user) {
        if (user) {
            window.email = user.email;
            window.userId = firebase.auth().currentUser.uid;
            // User is signed in.
            //console.log("signed in");
            signed_in_initially = true;
            $(".logoutButton").on('click touchstart', function () {
                firebase.auth().signOut().then(function () {
                    // Sign-out successful.
                }).catch(function (error) {
                    // An error happened.
                    handleError(error);
                });
            });
            $("#email").text(window.email);
            $("#bodycollapse").removeClass("collapse");
            $("#changeDoctorsCollapse").removeClass("collapse");
            getInitialValues();
            //console.log(window.userstatus);
            //console.log(window.userId);
            firebase.database().ref('users/' + window.userId).once('value').then(function (userData) {
                //console.log("getting user data");
                var signintype = userData.val().signintype;
                //console.log(signintype);
                if (signintype == "email") {
                    $("#changePasswordCollapse").removeClass("collapse");
                } else {
                    $("#changePasswordCollapse").addClass("collapse");
                }
            }).catch(function (err) {
                handleError(err);
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


    $(document).on('click touchstart', ".doctorDelete", function () {
        var valueArray = $(this).attr('value').split(',');
        var doctorKey = valueArray[0];
        var started = false;

        function doctorlist(doctorKey) {
            //console.log("delete the form");
            firebase.database().ref('users/' + window.userId + '/doctors/' + doctorKey).remove().then(function () {
                $("#doctordata").remove();
                $("#doctorlist").append("<tbody id=\"doctordata\"></tbody>");
                setTimeout(function () {
                    $('#alertconfirmdeletedoctor').fadeOut();
                }, config.other.alerttimeout);
                createDoctorTable();
            }).catch(function (error) {
                handleError(error);
            });
        }

        $('#alertconfirmdeletedoctor').fadeIn();
        $("#cancelDeleteDoctor").on('click touchstart', function () {
            if (!started) {
                $('#alertconfirmdeletedoctor').fadeOut();
                started = true;
            }
        });
        $("#confirmDeleteDoctor").on('click touchstart', function () {
            if (!started) {
                doctorlist(doctorKey);
                started = true;
            }
        });
    });

    function changePasswordSub() {
        if ($("#changePassword").valid()) {
            var user = firebase.auth().currentUser;
            var formdata = $("#changePassword").serializeArray();
            var newPass = formdata[0].value.toString();
            user.updatePassword(newPass).then(function () {
                // Update successful.
                //console.log("update success");
                $('#alertpasswordchangesuccess').fadeIn();
                setTimeout(function () {
                    $('#alertpasswordchangesuccess').fadeOut();
                }, config.other.alerttimeout);
            }).catch(function (error) {
                // An error happened.
                handleError(error);
            });
        }
    }

    function changeUsernameSub() {
        if ($("#changeUsername").valid()) {
            var formdata = $("#changeUsername").serializeArray();
            var newUsername = formdata[0].value.toString();
            firebase.database().ref('users/' + window.userId).update({
                username: newUsername
            }).then(function () {
                // Update successful.
                //console.log("update success");
                $('#alertusernamechangesuccess').fadeIn();
                setTimeout(function () {
                    $('#alertusernamechangesuccess').fadeOut();
                }, config.other.alerttimeout);
            }).catch(function (error) {
                // An error happened.
                handleError(error);
            });
        }
    }

    function changeNameSub() {
        if ($("#changeName").valid()) {
            var formdata = $("#changeName").serializeArray();
            var name = formdata[0].value.toString();
            var firstandlast = name.split(" ");
            var first = firstandlast[0];
            var last = firstandlast.slice(1).join(' ');
            firebase.database().ref('users/' + window.userId).update({
                name: name,
                firstname: first,
                lastname: last
            }).then(function () {
                // Update successful.
                //console.log("update success");
                $('#alertnamechangesuccess').fadeIn();
                setTimeout(function () {
                    $('#alertnamechangesuccess').fadeOut();
                }, config.other.alerttimeout);
            }).catch(function (error) {
                // An error happened.
                handleError(error);
            });
        }
    }

    $("#deleteAccount").on('click touchstart', function () {
        var button = $(this);
        //console.log(button);
        var valueArray = button.attr('value').split(',');
        //console.log(valueArray);
        var started = false;
        $('#alertconfirmdeleteaccount').fadeIn();
        $("#cancelDeleteAccount").on('click touchstart', function () {
            if (!started) {
                $('#alertconfirmdeleteaccount').fadeOut();
                started = true;
            }
        });
        $("#confirmDeleteAccount").on('click touchstart', function () {
            if (!started) {
                var user = firebase.auth().currentUser;
                firebase.database().ref('users/' + window.userId).remove().catch(function (err) {
                    handleError(err);
                }).then(function () {
                    user.delete().then(function () {
                        // User deleted.
                        //console.log("user deleted");
                        window.location.href = "login.html";
                    }).catch(function (error) {
                        // An error happened.
                        handleError(error);
                    });
                });
            }
        });
    });

    $("#changeNameSubmit").on('click touchstart', function () {
        changeNameSub();
    });

    $("#fullname").keypress(function (event) {
        if (event.which == '13') {
            event.preventDefault();
            changeNameSub();
        }
    });

    $("#changeUsernameSubmit").on('click touchstart', function () {
        changeUsernameSub();
    });

    $("#username").keypress(function (event) {
        if (event.which == '13') {
            event.preventDefault();
            changeUsernameSub();
        }
    });

    $("#changePasswordSubmit").on('click touchstart', function () {
        changePasswordSub();
    });

    $("#confirm_password").keypress(function (event) {
        if (event.which == '13') {
            event.preventDefault();
            changePasswordSub();
        }
    });

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

    $("#changeName").validate({
        rules: {
            fullname: {
                required: true,
                regex1: config.regex.fullname
            }
        },
        messages: {
            fullname: "Please enter your full name"
        },
        errorElement: "div",
        errorPlacement: function (error, element) {
            // Add the `invalid-feedback` class to the div element
            error.addClass("invalid-feedback");
            error.insertAfter(element);
        },
        highlight: function (element) {
            $(element).addClass("is-invalid").removeClass("is-valid");
        },
        unhighlight: function (element) {
            $(element).addClass("is-valid").removeClass("is-invalid");
        }
    });

    $("#changeUsername").validate({
        rules: {
            username: {
                required: true,
                minlength: 3,
                maxlength: 15
            }
        },
        messages: {
            username: {
                required: "Please enter a username",
                minlength: "Your username must consist of at least 3 characters",
                maxlength: "Your username cannot exceed 15 characters"
            }
        },
        errorElement: "div",
        errorPlacement: function (error, element) {
            // Add the `invalid-feedback` class to the div element
            error.addClass("invalid-feedback");
            error.insertAfter(element);
        },
        highlight: function (element) {
            $(element).addClass("is-invalid").removeClass("is-valid");
        },
        unhighlight: function (element) {
            $(element).addClass("is-valid").removeClass("is-invalid");
        }
    });

    $("#changePassword").validate({
        rules: {
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
            }
        },
        messages: {
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
            }
        },
        errorElement: "div",
        errorPlacement: function (error, element) {
            // Add the `invalid-feedback` class to the div element
            error.addClass("invalid-feedback");
            error.insertAfter(element);
        },
        highlight: function (element) {
            $(element).addClass("is-invalid").removeClass("is-valid");
        },
        unhighlight: function (element) {
            $(element).addClass("is-valid").removeClass("is-invalid");
        }
    });
});