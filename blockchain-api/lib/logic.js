'use strict';

/**
* Sample transaction
* @param {api.ChangeInsurance} ChangeInsurance
* @transaction
*/
function ChangeInsurance(changeInsurance) {
    if (!(changeInsurance.newInsurance.clients.includes(changeInsurance.patient))) {
        changeInsurance.newInsurance.clients.push(changeInsurance.patient);
    }
    changeInsurance.patient.insurance = changeInsurance.newInsurance;
    return getParticipantRegistry('api.Patient')
        .then(function (patientRegistry) {
            return patientRegistry.update(changeInsurance.patient);
        })
        .then(function () {
            return getParticipantRegistry('api.Insurance');
        })
        .then(function (insuranceRegistry) {
            return insuranceRegistry.update(changeInsurance.newInsurance);
        });
}

/**
* Sample transaction
* @param {api.RemoveDoctorViewAccess} RemoveDoctorViewAccess
* @transaction
*/
function RemoveDoctorViewAccess(doctorView) {
    return getParticipantRegistry('api.Doctor')
        .then(function (doctorRegistry) {
            var doctorIdStr = doctorView.doctorInfo.doctorIdStr;
            return doctorRegistry.get(doctorIdStr);
        }).then(function (doctor) {
            var doctorPatientsIndex = doctor.patients.indexOf(doctorView.patient);
            if (doctorPatientsIndex > -1) {
                doctor.patients.splice(doctorPatientsIndex, 1);
            }
            return getParticipantRegistry('api.Doctor')
            .then(function (docReg) {
                return docReg.update(doctor);
            }).then(function () {
                return getParticipantRegistry('api.Patient');
            }).then(function (patientRegistry) {
                var patientDoctorIndex = doctorView.patient.doctors.indexOf(doctorView.doctorInfo);
                if (patientDoctorIndex > -1) {
                    doctorView.patient.doctors.splice(patientDoctorIndex, 1);
                }
                return patientRegistry.update(doctorView.patient);
            });
        });
}

/**
* Sample transaction
* @param {api.AddDoctorViewAccess} AddDoctorViewAccess
* @transaction
*/
function AddDoctorViewAccess(doctorView) {
    if (doctorView.doctor.patients.indexOf(doctorView.patient) < 0) {
        doctorView.doctor.patients.push(doctorView.patient);
    }
    console.log("test1");
    return getParticipantRegistry('api.Doctor')
        .then(function (doctorRegistry) {
            return doctorRegistry.update(doctorView.doctor);
        }).then(function () {
            return getParticipantRegistry('api.Patient');
        }).then(function (patientRegistry) {
            if (doctorView.patient.doctors.indexOf(doctorView.doctor.doctorInfo) < 0) {
                doctorView.patient.doctors.push(doctorView.doctor.doctorInfo);
            }
            return patientRegistry.update(doctorView.patient);
        });
}

/**
* Sample transaction
* @param {api.DoctorUpdatePatientData} DoctorUpdatePatientData
* @transaction
*/
function DoctorUpdatePatientData(EHRData) {
    if (EHRData.patient.doctors.indexOf(EHRData.doctor.doctorInfo) > -1) {
        if (EHRData.prescription !== "") {
            EHRData.patient.ehr.prescription = EHRData.prescription;
        }
        if (EHRData.age !== "") {
            EHRData.patient.ehr.age = EHRData.age;
        }
        if (EHRData.diagnosis !== "") {
            EHRData.patient.ehr.diagnosis = EHRData.diagnosis;
        }
        if (EHRData.notes !== "") {
            EHRData.patient.ehr.notes = EHRData.notes;
        }
        if (EHRData.otherdata !== "") {
            EHRData.patient.ehr.otherdata = EHRData.otherdata;
        }
        return getAssetRegistry('api.EHR')
            .then(function (ehrRegistry) {
                return ehrRegistry.update(EHRData.patient.ehr);
            });
    } else {
        throw new Error("Access Denied");
    }
}