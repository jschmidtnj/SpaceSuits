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
    if (error.code !== "" && error.message !== "") {
        customMessage = errorMessage;
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

window.tasksjson = {};

$(document).ready(function () {

    $('#toslink').attr('href', config.other.tosUrl);
    $('#privacypolicylink').attr('href', config.other.privacyPolicyUrl);
    $('#helplink').attr('href', config.other.helpPageUrl);

    $.validator.addMethod(
        "regex",
        function (value, element, regexp) {
            var re = new RegExp(regexp, 'i');
            return this.optional(element) || re.test(value);
        },
        ""
    );

    var tableinitialized = false;

    function createTaskTable() {
        //console.log(window.tasksjson);
        if (Object.keys(window.tasksjson).length === 0) {
            $("#taskdata").remove();
            $("#tasklist").append("<tbody id=\"taskdata\"></tbody>");
            $("#tasklistcollapse").addClass("collapse");
            $("#notaskswarning").removeClass("collapse");
            tableinitialized = false;
            $('#tasklist').DataTable().destroy();
            return;
        }
        for (var majorkey in window.tasksjson) {
            for (var subkey in window.tasksjson[majorkey]) {
                var singletaskdata = window.tasksjson[majorkey][subkey];
                var tasktimeutc = parseInt(singletaskdata.time);
                //console.log("time: " + tasktimeutc.toString());
                var tasktimeutcdate = new Date(tasktimeutc);
                var singletasktime = tasktimeutcdate.toString();
                //console.log("date: " + singletasktime);  
                var singletaskdatavalue = singletaskdata.data;
                //console.log(majorkey + " -> " + subkey + " -> " + singletaskdatavalue);
                var tasknum = majorkey + "." + subkey;
                $('#taskdata').append("<tr><td>" + tasknum + "</td><td>" + singletaskdatavalue +
                    "</td><td>" + singletasktime + "</td><td><button value=\"" + tasknum +
                    "\" class=\"taskDelete btn btn-primary btn-block onclick=\"" +
                    "void(0)\"\">Delete</button></td></tr>");
            }
        }
        $("#notaskswarning").addClass("collapse");
        $("#tasklistcollapse").removeClass("collapse");
        if (!(tableinitialized)) {
            $('#tasklist').DataTable({
                responsive: true
            });
            tableinitialized = true;
        }
    }

    function updateTasks() {
        var updatedtasksjsonstr = JSON.stringify(window.tasksjson);
        //console.log(updatedtasksjsonstr);
        firebase.database().ref('tasks').set({
            data: updatedtasksjsonstr
        }).then(function () {
            //console.log("task added");
            $('#task').val('');
            $('#alerttaskupdatesuccess').fadeIn();
            setTimeout(function () {
                $('#alerttaskupdatesuccess').fadeOut();
            }, config.other.alerttimeout);
        }).catch(function (err) {
            handleError(err);
        });
    }

    function initializeTaskForm() {
        $("#taskForm").validate({
            rules: {
                task: {
                    required: true,
                    regex: config.regex.validtask
                }
            },
            messages: {
                task: {
                    required: "Please enter a task",
                    regex: "Please enter task in format <1.0 taskinfo>"
                }
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

        function getInitialTasks() {
            firebase.database().ref('tasks/data').once('value').then(function (taskdata) {
                var taskdataval = taskdata.val();
                //console.log(taskdataval);
                if (taskdataval === undefined || taskdataval === "" || taskdataval == null) {
                    taskdataval = "{}";
                }
                window.tasksjson = JSON.parse(taskdataval);
                //console.log(window.tasksjson);
            }).then(function () {
                $("#taskdata").remove();
                $("#tasklist").append("<tbody id=\"taskdata\"></tbody>");
                createTaskTable();
            }).catch(function (err) {
                handleError(err);
            });
        }

        function submitTask() {
            if ($("#taskForm").valid()) {
                var formData = $("#taskForm").serializeArray();
                //console.log(formData);
                var completetask = formData[0].value.toString();
                var tasksplit = completetask.split(" ");
                var tasknum = tasksplit[0];
                var tasknumsplit = tasknum.split(".");
                var majortasknum = tasknumsplit[0];
                var subtasknum = tasknumsplit[1];
                delete tasksplit[0];
                var taskvalue = tasksplit.join(' ').trim();
                var timestamp = Date.now().toString();
                var subtaskdata = {
                    "data": taskvalue,
                    "time": timestamp
                };
                var currenttaskdata = window.tasksjson[majortasknum];
                if (currenttaskdata == undefined) {
                    window.tasksjson[majortasknum] = {};
                }
                window.tasksjson[majortasknum][subtasknum] = subtaskdata;
                //console.log(window.tasksjson);
                updateTasks();
                $("#taskdata").remove();
                $("#tasklist").append("<tbody id=\"taskdata\"></tbody>");
                createTaskTable();
            } else {
                //console.log("form invalid");
                handleError({
                    message: "form invalid",
                    code: "tast-invalid"
                });
            }
        }

        getInitialTasks();

        var submittedTask = false;
        $("#submitTask").on('click touchstart', function (e) {
            e.stopImmediatePropagation();
            if (e.type == "touchstart") {
                submittedTask = true;
                submitTask();
            }
            else if (e.type == "click" && !submittedTask) {
                submitTask();
            }
            else {
                submittedTask = false;
            }
            return false;
        });

        $("#task").keypress(function (event) {
            if (event.which == '13') {
                event.preventDefault();
                submitTask();
            }
        });

        var taskdata = firebase.database().ref("tasks");
        taskdata.on("value", function (tasks) {
            var taskdataval = tasks.val();
            var tasksjson = taskdataval.data;
            if (tasksjson !== JSON.stringify(window.tasksjson) && tableinitialized) {
                //console.log("update tasks table if new data");
                window.tasksjson = JSON.parse(tasksjson);
                $("#taskdata").remove();
                $("#tasklist").append("<tbody id=\"taskdata\"></tbody>");
                createTaskTable();
            }
        });

    }

    function getSuitData() {
        //console.log("get the suit data");
        var suitdataref = firebase.database().ref("suitdata");
        suitdataref.on("value", function (suitdata) {
            var suitdataval = suitdata.val();
            //console.log(suitdataval);
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

    $(document).on('click touchstart', ".taskDelete", function () {
        var valueArray = $(this).attr('value').split(',');
        var taskKey = valueArray[0];
        var started = false;

        function deleteTask(taskKey) {
            //console.log("delete the task");
            var taskKeySplit = taskKey.split(".");
            var majorTaskKey = taskKeySplit[0];
            var subTaskKey = taskKeySplit[1];
            delete window.tasksjson[majorTaskKey][subTaskKey];
            if (Object.keys(window.tasksjson[majorTaskKey]).length === 0) {
                delete window.tasksjson[majorTaskKey];
            }
            updateTasks();
            $("#taskdata").remove();
            $("#tasklist").append("<tbody id=\"taskdata\"></tbody>");
            setTimeout(function () {
                $('#alertconfirmdeletetask').fadeOut();
            }, config.other.alerttimeout);
            createTaskTable();
        }

        $('#alertconfirmdeletetask').fadeIn();

        $("#cancelDeleteTask").on('click touchstart', function () {
            if (!started) {
                $('#alertconfirmdeletetask').fadeOut();
                started = true;
            }
            return false;
        });

        $("#confirmDeleteTask").on('click touchstart', function () {
            if (!started) {
                deleteTask(taskKey);
                started = true;
            }
            return false;
        });

        return false;
    });

    var signed_in_initially = false;

    firebase.auth().onAuthStateChanged(function (user) {
        if (user) {
            window.user = user;
            signed_in_initially = true;
            firebase.database().ref('users/' + user.uid).once('value').then(function (userdata) {
                var userdataval = userdata.val();
                var usertype = userdataval.usertype;
                if (usertype == "groundcontrol") {
                    //console.log("ground control signed in");
                }
            }).catch(function (err) {
                handleError(err);
            });
            $("#bodycollapse").removeClass("collapse");
            $("#taskmanagementcollapse").removeClass("collapse");
            initializeTaskForm();
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

    var handledLogout = false;
    $("#logoutButton").on('click touchstart', function () {
        e.stopImmediatePropagation();
        if (e.type == "touchstart") {
            handledLogout = true;
            handleLogout();
        }
        else if (e.type == "click" && !handledLogout) {
            handleLogout();
        }
        else {
            handledLogout = false;
        }
        return false;
    });

    function handleLogout() {
        firebase.auth().signOut().then(function () {
            // Sign-out successful.
        }).catch(function (error) {
            // An error happened.
            handleError(error);
        });
    }

});