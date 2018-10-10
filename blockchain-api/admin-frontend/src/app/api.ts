import {Asset} from './org.hyperledger.composer.system';
import {Participant} from './org.hyperledger.composer.system';
import {Transaction} from './org.hyperledger.composer.system';
import {Event} from './org.hyperledger.composer.system';
// export namespace api{
   export class EHR extends Asset {
      ehrId: string;
      age: number;
      prescription: string;
      diagnosis: string;
      notes: string;
      otherdata: string;
   }
   export class PatientInfo extends Asset {
      patientInfoId: string;
      firstName: string;
      lastName: string;
   }
   export class DoctorInfo extends Asset {
      doctorInfoId: string;
      doctorIdStr: string;
      firstName: string;
      lastName: string;
   }
   export class Patient extends Participant {
      patientId: string;
      patientInfo: PatientInfo;
      insurance: Insurance;
      doctors: DoctorInfo[];
      ehr: EHR;
   }
   export class Doctor extends Participant {
      doctorId: string;
      doctorInfo: DoctorInfo;
      patients: Patient[];
   }
   export class Insurance extends Participant {
      insuranceId: string;
      firstName: string;
      lastName: string;
      clients: Patient[];
   }
   export class ChangeInsurance extends Transaction {
      patient: Patient;
      newInsurance: Insurance;
   }
   export class RemoveDoctorViewAccess extends Transaction {
      patient: Patient;
      doctorInfo: DoctorInfo;
   }
   export class AddDoctorViewAccess extends Transaction {
      patient: Patient;
      doctor: Doctor;
   }
   export class DoctorUpdatePatientData extends Transaction {
      patient: Patient;
      doctor: Doctor;
      age: number;
      prescription: string;
      diagnosis: string;
      notes: string;
      otherdata: string;
   }
// }
