var config = {
    "mailjet": {
        "mailjetapikey": "key",
        "mailjetapisecret": "secret"
    },
    "firebaseadmin": {
        "type": "service_account",
        "project_id": "app",
        "private_key_id": "key",
        "private_key": "-----BEGIN PRIVATE KEY-----\nkey\n-----END PRIVATE KEY-----\n",
        "client_email": "email@app.iam.gserviceaccount.com",
        "client_id": "123456789",
        "auth_uri": "https://accounts.google.com/o/oauth2/auth",
        "token_uri": "https://oauth2.googleapis.com/token",
        "auth_provider_x509_cert_url": "https://www.googleapis.com/oauth2/v1/certs",
        "client_x509_cert_url": "url"
    },
    "other": {
        "databaseurl": "url",
        "datatimeout": 1000,
        "maxconcurrentaccountdeletions": "3",
        "daysofinactivity": "1",
        "accountcleanupcronjoburl": "url",
        "contactcleanupcronjoburl": "url",
        "secretKey": "key"
    }
};

module.exports = config;