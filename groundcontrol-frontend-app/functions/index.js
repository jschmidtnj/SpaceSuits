const functions = require('firebase-functions');
const config = require('./config/config');
const admin = require('firebase-admin');
admin.initializeApp({
    credential: admin.credential.cert(config.firebaseadmin),
    databaseURL: config.other.databaseurl
});

const promisePool = require('es6-promise-pool');
const PromisePool = promisePool.PromisePool;
const secureCompare = require('secure-compare');

var onedaymilli = 24 * 60 * 60 * 1000;
var daysofinactivity = parseInt(config.other.daysofinactivity);

var secretKey = config.other.secretKey;

//see https://github.com/firebase/functions-samples/tree/master/delete-unused-accounts-cron for more details
exports.accountcleanup = functions.https.onRequest((req, res) => {
    //console.log("cleaning up users");
    //console.log(config.other);
    var MAX_CONCURRENT = parseInt(config.other.maxconcurrentaccountdeletions);
    //console.log(MAX_CONCURRENT);
    const key = req.query.key;
    // Exit if the keys don't match.
    if (!secureCompare(key, functions.config().cron.key)) {
        console.log('The key provided in the request does not match the key set in the environment. Check that', key,
            'matches the cron.key attribute in `firebase env:get`');
        res.status(403).send('Security key does not match. Make sure your "key" URL query parameter matches the ' +
            'cron.key environment variable.');
        return null;
    }

    // Fetch all user details.
    return getInactiveUsers().then((inactiveUsers) => {
        // Use a pool so that we delete maximum `MAX_CONCURRENT` users in parallel.
        const promisePool = new PromisePool(() => deleteInactiveUser(inactiveUsers), MAX_CONCURRENT);
        return promisePool.start();
    }).then(() => {
        //console.log('User cleanup finished');
        res.send('User cleanup finished');
        return null;
    });
});

/**
 * Deletes one inactive user from the list.
 */
function deleteInactiveUser(inactiveUsers) {
    if (inactiveUsers.length > 0) {
        const userToDelete = inactiveUsers.pop();

        // Delete the inactive user.
        return admin.auth().deleteUser(userToDelete.uid).then(() => {
            admin.database().ref("users/" + userToDelete.uid).remove();
            //console.log('Deleted user account', userToDelete.uid, 'because of inactivity and email is not verified');
            return null;
        }).catch(error => {
            console.error('Deletion of inactive user account', userToDelete.uid, 'failed:', error);
            return null;
        });
    }
    return null;
}

/**
 * Returns the list of all inactive users.
 */
function getInactiveUsers(users = [], nextPageToken) {
    return admin.auth().listUsers(1000, nextPageToken).then((result) => {
        // Find users that have not signed in in the last 30 days.
        const inactiveUsers = result.users.filter(
            user => Date.parse(user.metadata.lastSignInTime) < (Date.now() - onedaymilli * daysofinactivity) && !(user.emailVerified));
        /*
        const activeUsers = result.users.filter(
            user => Date.parse(user.metadata.lastSignInTime) >= (Date.now() - onedaymilli * daysofinactivity) && user.emailVerified);
        //console.log("active users:");
        //console.log(activeUsers);
        for (var i = 0; i < activeUsers.length; i++) {
            //console.log(activeUsers[i].email);
            //console.log(Date.parse(activeUsers[i].metadata.lastSignInTime));
            //console.log(Date.now() - 30 * 24 * 60 * 60 * 1000);
            //console.log("email verified: " + !(activeUsers[i].emailVerified));
            //console.log("date config: " + Date.parse(activeUsers[i].metadata.lastSignInTime) < (Date.now() - onedaymilli * config.other.daysofinactivity));
        }
        */
        // Concat with list of previously found inactive users if there was more than 1000 users.
        /*
        //console.log("inactive users:");
        if (inactiveUsers.length > 0) {
            //console.log(inactiveUsers);
        }
        for (var i = 0; i < inactiveUsers.length; i++) {
            //console.log(inactiveUsers[i].email);
        }
        */
        users = users.concat(inactiveUsers);
        //console.log(users);
        // If there are more users to fetch we fetch them.
        if (result.pageToken) {
            return getInactiveUsers(users, result.pageToken);
        }
        return users;
    });
}

exports.checksecretkey = functions.https.onCall((data, context) => {
    const userSecretKey = data.secretKey;
    var status = "invalid";
    if (userSecretKey === secretKey) {
        status = "valid";
    }
    return {
        status: status
    };
});